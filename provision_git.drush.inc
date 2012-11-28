<?php

/**
 * @file
 * Provision/Drush hooks for git commands.
 */

/**
 * Implementation of hook_drush_command().
 */
function provision_git_drush_command() {
  $items['provision-git-pull'] = array(
    'description' => 'Git pull a platform directory.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'options' => array(
      'reset' => 'Resets any working tree changes that would block a rebase.  USE WITH CAUTION',
    ),
    'aliases' => array('pull'),
  );
  $items['provision-git-push'] = array(
    'description' => 'Git push a platform directory.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'aliases' => array('push'),
  );
  $items['provision-git-commit'] = array(
    'description' => 'Commit all working copy files to the repository.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'aliases' => array('commit'),
    'options' => array(
      'message' => 'The message to attach to the commit.',
    ),
  );
  $items['provision-git-reset'] = array(
    'description' => 'Git reset platform directory.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'aliases' => array('reset'),
  );
  $items['provision-git-add'] = array(
    'description' => 'Add files/directories to the Git index.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'aliases' => array('add'),
    'arguments' => array(
      'path' => 'Path to the file or folders you would like to add.',
    ),
    'required-arguments' => TRUE,
  );
  return $items;
}

/**
 * Helper for verifying a repo
 */
function provision_git_is_repo($repo_path = NULL){
  if (!$repo_path){  
    if (d()->type === 'site') {
      $repo_path = d()->site_path? d()->site_path: d()->root;
    }
    elseif (d()->type == 'platform') {
      $repo_path = d()->platform->root? d()->platform->root: d()->root;
    }
  }
  
  //If shell returns False, this is not a git repo
  if (!drush_shell_cd_and_exec($repo_path, 'git status')){
    drush_set_error('DRUSH_PROVISION_GIT_PULL_FAILED', dt("Git repository not found in: !path", array('!path' => $repo_path)));
    return FALSE;
  } else {
     // Find the toplevel directory for this git repository
    if (drush_shell_cd_and_exec($repo_path, 'git rev-parse --show-toplevel')){
      $repo_path = implode("\n", drush_shell_exec_output());
    }
    drush_log(dt("Git repository found in !path", array('!path' => $repo_path)), 'ok');
    drush_set_option('repo_path', $repo_path);
    return TRUE;
  }
}

/**
 * Pre provision-git-pull hook
 */
function drush_provision_pre_provision_git_pull(){
  provision_git_is_repo();
}

/**
 * Pre provision-git-push hook
 */
function drush_provision_pre_provision_git_push(){
  provision_git_is_repo();
}

/**
 * Pre provision-git-commit hook
 */
function drush_provision_pre_provision_git_commit(){
  provision_git_is_repo();
}
/**
 * Pre provision-git-reset hook
 */
function drush_provision_pre_provision_git_reset(){
  provision_git_is_repo();
}
/**
 * Pre provision-git-add hook
 */
function drush_provision_pre_provision_git_add(){
  provision_git_is_repo();
}

/**
 * Implements the provision-git-pull command.
 */
function drush_provision_git_pull() {
  $repo_path = drush_get_option('repo_path');
  $target = d()->name;
  
  // Only git reset --hard if option says so.
  if (drush_get_option('reset') == TRUE){
    provision_backend_invoke($target, 'provision-git-reset');
  }
  
  // Execute git pull --rebase
  // @TODO: make rebase optional as well??  Haven't thought this through yet.
  if (drush_shell_cd_and_exec($repo_path, 'git pull --rebase')) {
    drush_log(dt('Git repository pulled.', array('!path' => $repo_path)), 'ok');
    drush_log(implode("\n", drush_shell_exec_output()), 'ok');

    // Submodule in git? lets update those.
    if (file_exists($repo_path . '/.gitmodules')) {
      drush_shell_cd_and_exec($repo_path, 'git submodule init');
      drush_log(implode("\n", drush_shell_exec_output()), 'ok');
      drush_shell_cd_and_exec($repo_path, 'git submodule update');
      drush_log(implode("\n", drush_shell_exec_output()), 'ok');
    }
  }
  else {
    return drush_set_error('DRUSH_PROVISION_GIT_PULL_FAILED', dt("Git pull failed in !path.\nThe specific errors are below:\n!errors", array('!path' => $repo_path, '!errors' => implode("\n", drush_shell_exec_output()))));
  }
}

/**
 * Implements the provision-git-push command.
 */
function drush_provision_git_push() {
  $repo_path = drush_get_option('repo_path');
  $target = d()->name;
  
  //Execute git push
  if (drush_shell_cd_and_exec($repo_path, 'git push')) {
    drush_log(dt('Git repository pushed.'), 'ok');
    drush_log(implode("\n", drush_shell_exec_output()), 'ok');
  }
  else {
    return drush_set_error('DRUSH_PROVISION_GIT_PULL_FAILED', dt("Git push failed in !path.\nThe specific errors are below:\n!errors", array('!path' => $repo_path, '!errors' => implode("\n", drush_shell_exec_output()))));
  }
}

/**
 * Implements the provision-git-commit command.
 */
function drush_provision_git_commit() {
  $repo_path = drush_get_option('repo_path');
  $target = d()->name;
  
  //Generate commit message
  $message = array();
  $message[] = "Provision Git Commit: $target";
  $message[] = str_repeat('=', strlen($message[0]));
  if (drush_get_option('message')){
    $message[] = drush_get_option('message');
  }
  $message = implode("\n", $message);

  //Execute git commit 
  $ec = drush_shell_cd_and_exec($repo_path, "git commit -am '$message'");

  // drush_sell_cd_and_exec() returns TRUE on a exist status of 0, and
  // FALSE on a non-zero exit status. Unfortunately, git exits with
  // a status of 1 if there are no files to commit. This causes
  // drush_shell_cd_and_exec() to return FALSE and we think the
  // commit failed. So we have to resort to some hackery. We scan
  // the output and count how many files don't starts with '#'.
  // if the count is 1, then we have to assume there was nothing
  // to commit.

  // Assume nothing committed
  drush_set_context('PROVISION_GIT_COMMITTED', 0);	

  if (!$ec) {
    $comments = 0;
    $lines = 0;

    foreach(drush_shell_exec_output() as $line) {
      if ($line[0] == '#') {
        $comments++;
      }
      else {
        $lines++;
      }
    }
    if ($comments > 0 && $lines == 1) {
      drush_log(dt('Nothing to commit.'), 'ok');
      drush_set_context('DRUSH_ERROR_CODE', DRUSH_SUCCESS);
    }
    else {
      return drush_set_error('DRUSH_PROVISION_GIT_COMMIT_FAILED', dt("Git commit failed in !path.\nThe specific errors are below:\n!errors", array('!path' => $repo_path, '!errors' => implode("\n", drush_shell_exec_output()))));
    }
  }
  else {
    drush_set_context('PROVISION_GIT_COMMITTED', 1);
    drush_log(dt('Git repository committed.'), 'ok');
    drush_log(implode("\n", drush_shell_exec_output()), 'ok');
  }
}

/**
 * Implements the provision-git_pull command.
 */
function drush_provision_git_reset() {
  $repo_path = drush_get_option('repo_path');
  $target = d()->name;
  
  //Call git reset --hard which resets to the last pulled commit
  if (drush_shell_cd_and_exec($repo_path, 'git reset --hard')){
    drush_log(dt('Git repository reset.'), 'ok');
    if (file_exists($repo_path . '/.gitmodules')) {
      if (drush_shell_cd_and_exec($repo_path, 'git submodule foreach \'git reset --hard\'')){
        drush_log(dt('Git submodules reset.'), 'ok');
      }
    }
  }
}

/**
 * Implements the provision-git_add command.
 */
function drush_provision_git_add($path = '') {
  $repo_path = drush_get_option('repo_path');
  $target = d()->name;

  // Call git add.
  // Seems to pass even if the git add failed
  if (drush_shell_cd_and_exec($repo_path, "git add $path")){
    drush_log(dt('Git add: '. $path), 'ok');
  }
}
