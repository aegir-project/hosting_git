<?php

/**
 * @file
 * Provision/Drush hooks for git commands.
 */

/**
 * Implementation of hook_drush_command().
 */
function provision_git_drush_command() {
  $items['provision-git-fetch'] = array(
    'description' => 'Executes "git fetch".',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'options' => array(
      'repo_path' => 'Optional: force a repository path. Defaults to the site or platform dir.',
      'force_repo_path_as_toplevel' => 'Stop Git from looking in parent directories. Defaults to FALSE.',
    ),
    'aliases' => array('fetch'),
  );
  $items['provision-git-push'] = array(
    'description' => 'Executes "git push".',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'aliases' => array('push'),
    'options' => array(
      'repo_path' => 'Optional: force a repository path. Defaults to the site or platform dir.',
      'force_repo_path_as_toplevel' => 'Stop Git from looking in parent directories. Defaults to FALSE.',
    ),
  );
  $items['provision-git-commit'] = array(
    'description' => 'Commits all added and changed files to the repository.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'aliases' => array('commit'),
    'options' => array(
      'message' => 'The message to attach to the commit.',
      'repo_path' => 'Optional: force a repository path. Defaults to the site or platform dir.',
      'force_repo_path_as_toplevel' => 'Stop Git from looking in parent directories. Defaults to FALSE.',
    ),
  );
  $items['provision-git-reset'] = array(
    'description' => 'Executes "git reset --hard".',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'aliases' => array('reset'),
    'options' => array(
      'repo_path' => 'Optional: force a repository path. Defaults to the site or platform dir.',
      'force_repo_path_as_toplevel' => 'Stop Git from looking in parent directories. Defaults to FALSE.',
    ),
  );
  $items['provision-git-add'] = array(
    'description' => 'Adds files and directories to be committed.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'aliases' => array('add'),
    'arguments' => array(
      'path' => 'Path to the file or folders you would like to add.',
    ),
    'required-arguments' => TRUE,
    'options' => array(
      'repo_path' => 'Optional: force a repository path. Defaults to the site or platform dir.',
      'force_repo_path_as_toplevel' => 'Stop Git from looking in parent directories. Defaults to FALSE.',
    ),
  );
  $items['provision-git-clone'] = array(
    'description' => 'Git clone a repo in a specified location.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'arguments' => array(
      'git_url' => dt('The git repository to clone.'),
      'path' => dt('The location where the git repository will be cloned.'),
      'git_ref' => dt('Parameter for the git checkout command'),
    ),
  );

  // Other commands are defined in sub modules.

  return $items;
}

/**
 * Helper for verifying the chosen path is a git repo.
 *
 * @param bool $set_error
 *   Use drush_set_error when it's not a repo, default to TRUE.
 *   Set to FALSE to silently check if a directory is a Git repo.
 *
 * @return bool
 *   TRUE if it's a functional Git repository, FALSE otherwise.
 */
function provision_git_is_repo($repo_path = NULL, $set_error = TRUE) {
  // If no path specified, use the site or platform path of the alias.
  if (!$repo_path) {
    $repo_path = drush_get_option('repo_path');
  }
  if (!$repo_path) {
    if (d()->type === 'site') {
      $repo_path = d()->site_path ? d()->site_path : d()->root;
    } elseif (d()->type == 'platform') {
      $repo_path = d()->platform->root ? d()->platform->root : d()->root;
    }
  }

  // Do not allow /var/aegir to be repo_path.
  // See https://www.drupal.org/node/2893588
  if ($repo_path == '/var/aegir') {
    $repo_path = NULL;
    drush_set_option('repo_path', $repo_path);
    return drush_set_error('PROVISION_GIT_ERROR', dt('Warning: The "repo_path" for this !type is set to "/var/aegir". This is not allowed. Please manually edit the drush alias file in /var/aegir/.drush/!file.alias.drushrc.php.  Option repo_path has been unset.', array(
      '!file' => ltrim(d()->name, '@'),
      '!type' => d()->type,
    )));
  }

  // If repo_path does not exist, or git status returns False, this is not a git repo
  if (!file_exists($repo_path . '/.git/config') || !drush_shell_cd_and_exec($repo_path, 'git status')) {
    if ($set_error) {
      drush_set_error('DRUSH_PROVISION_GIT_PULL_FAILED', dt("Git repository not found in: !path", array('!path' => $repo_path)));
    }
    return FALSE;
  } else {
    // Find the toplevel directory for this git repository
    if (drush_shell_cd_and_exec($repo_path, 'git rev-parse --show-toplevel')) {
      $top_repo_path = implode("\n", drush_shell_exec_output());
    }
    if (drush_get_option('force_repo_path_as_toplevel', FALSE) && realpath($repo_path) != $top_repo_path) {
      if ($set_error) {
        drush_set_error('DRUSH_PROVISION_GIT_PULL_FAILED', dt("Path !path is not the toplevel dir of this Git repository.", array('!path' => $repo_path)));
      }
      return FALSE;
    }
    drush_log(dt("Git repository found in !path", array('!path' => $top_repo_path)), 'ok');
    drush_set_option('repo_path', $top_repo_path);
    return TRUE;
  }
}

/**
 * Pre provision-git-push hook
 */
function drush_provision_pre_provision_git_push() {
  provision_git_is_repo();
}

/**
 * Pre provision-git-commit hook
 */
function drush_provision_pre_provision_git_commit() {
  provision_git_is_repo();
}

/**
 * Pre provision-git-reset hook
 */
function drush_provision_pre_provision_git_reset() {
  provision_git_is_repo();
}

/**
 * Pre provision-git-add hook
 */
function drush_provision_pre_provision_git_add() {
  provision_git_is_repo();
}

/**
 * Pre provision-git-clone hook
 */
function drush_provision_pre_provision_git_clone() {
  // Nothing to do here.
}

/**
 * Implements the provision-git-fetch command.
 */
function drush_provision_git_fetch() {
  $repo_path = drush_get_option('repo_path');
  $target = d()->name;

  if (drush_shell_cd_and_exec($repo_path, 'git fetch')) {
    drush_log(dt('Git repository !path fetched.', array('!path' => $repo_path)), 'ok');
    $out = drush_shell_exec_output();
    if ($out) {
      drush_log(implode("\n", $out), 'ok');
    }
  }
  else {
    return drush_set_error('DRUSH_PROVISION_GIT_FETCH_FAILED', dt("Git fetch failed in !path.\nThe specific errors are below:\n!errors", array('!path' => $repo_path, '!errors' => implode("\n", drush_shell_exec_output()))));
  }
}

/**
 * Implements the provision-git-push command.
 */
function drush_provision_git_push() {
  $repo_path = drush_get_option('repo_path');
  $target = d()->name;

  // Execute git push
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
  if (drush_get_option('message')) {
    $message[] = str_repeat('=', strlen($message[0]));
    $message[] = drush_get_option('message');
  }
  $message = implode("\n", $message);

  // Execute git commit
  // @TODO: Decide if we should we always assume "-a" (commit all changes)?
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

    foreach (drush_shell_exec_output() as $line) {
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
 * Implements the provision-git-reset command.
 */
function drush_provision_git_reset() {
  $repo_path = drush_get_option('repo_path');
  $target = d()->name;

  // Call git reset --hard which resets to the last pulled commit
  if (drush_shell_cd_and_exec($repo_path, 'git reset --hard')) {
    drush_log(dt('Git repository reset.'), 'ok');
    if (file_exists($repo_path . '/.gitmodules')) {
      if (drush_shell_cd_and_exec($repo_path, 'git submodule foreach \'git reset --hard\'')) {
        drush_log(dt('Git submodules reset.'), 'ok');
      }
    }
  }
}

/**
 * Implements the provision-git-clone command.
 */
function drush_provision_git_clone($git_url = '', $path = '', $git_ref = '') {

  // Get URL and path from d()
  // @TODO: Move to a validate hook.
  if (!$git_url) {
    $git_url = d()->repo_url;
  }
  if (!$path) {
    switch (d()->type) {
      case 'site':
        $path = d()->site_path;
        break;
      case 'platform':
      default:
        $path = d()->repo_path;
        break;
    }
  }
  if (!$git_ref) {
    $git_ref = d()->git_ref;
  }

  drush_log(dt('Cloning !url to !path', array('!url' => $git_url, '!path' => $path, 'ok')));

  if (is_dir($path)) {
    return drush_set_error('DRUSH_PROVISION_GIT_CLONE_FAILED', dt('Directory already exists.'));
  }

  $command = 'git clone --recursive ';
  $command .= escapeshellarg(trim($git_url));
  $command .= ' ' . escapeshellarg(trim($path));

  if ($git_ref) {
    $command .= ' --branch ' . escapeshellarg(trim($git_ref));
  }

  // Try to perform the git clone
  drush_log("Running $command", 'ok');
  if (drush_shell_exec($command)) {
    // Log the fact that it worked
    drush_log(dt('Clone successful.'), 'ok');
    drush_log(implode("\n", drush_shell_exec_output()), 'ok');
  }
  else {
    // The command failed
    return drush_set_error('DRUSH_PROVISION_GIT_CLONE_FAILED', dt("Git clone failed! \nThe specific errors are below:\n!errors", array('!errors' => implode("\n", drush_shell_exec_output()))));
  }
}

/**
 * Implements the provision-git-add command.
 */
function drush_provision_git_add($path = '') {
  $repo_path = drush_get_option('repo_path');
  $target = d()->name;

  // Call git add.
  // Seems to pass even if the git add failed
  if (drush_shell_cd_and_exec($repo_path, "git add $path")) {
    drush_log(dt('Git add: ' . $path), 'ok');
  }
}


/**
 * Implements drush_hook_provision_verify_validate().
 *
 * This needs to be called in the validate step so that it runs before the
 * Makefile check and the check for a Drupal installation. Those operations,
 * very inconveniently, run in the same function.
 */
function drush_provision_git_provision_verify_validate() {
  if (in_array(d()->type, array('site', 'platform')) && d()->deploy_from_git) {

    switch (d()->type) {
      case 'site':
        $context_type = 'Site';
        $repo_path = d()->site_path;
        $path_exists = provision_file()->exists(d()->site_path)->status();
        // If there's an issue with the site git repo, we end up doing a
        // checkout on the platform, if we aren't careful.
        drush_set_option('force_repo_path_as_toplevel', TRUE);
        break;

      case 'platform':
      default:
        $context_type = 'Platform';
        $repo_path = d()->repo_path;
        $path_exists = provision_file()->exists(d()->repo_path)->status();
        break;
    }
    // If the platform path doesn't exist, we will clone it.
    // Sites aren't verified on install, so we need to clone the repo in
    // drush_provision_git_provision_install_validate() instead.
    if (!$path_exists) {
      drush_log(dt("$context_type path does not exist. Invoking provision-git-clone."), 'ok');
      provision_backend_invoke(d()->name, 'provision-git-clone');
    }

    // Path already exists:  check if remotes match.
    else {
      // If our repo_url is not found as a remote, show a warning.
      drush_shell_cd_and_exec($repo_path, 'git remote -v');
      $output = drush_shell_exec_output();
      if (strpos(implode(',', $output), d()->repo_url) === FALSE) {
        drush_log(dt('The git repository located at !path did not have the repository !remote set as a remote. Please check the platform repo URL and git remotes and try again.', [
          '!path' => $repo_path,
          '!remote' => d()->repo_url,
        ]), 'warning');
      }
      else {
        drush_log(dt('Clone of !remote found at !path. Continuing verification.', [
          '!path' => $repo_path,
          '!remote' => d()->repo_url,
        ]), 'success');
      }
    }
  }
  else {
    // Check to see if we can puick up on existing Git info.
    if (provision_git_is_repo(NULL, FALSE)) {
      _provision_git_update_git_meta_data();
    }
  }
}

/**
 * Implements drush_hook_provision_install_validate().
 *
 * The site directory is created in an implementation of
 * drush_hook_pre_COMMAND(). So to clone the site repo first, we need to do it
 * in an earlier hook.
 */
function drush_provision_git_provision_install_validate() {
  if (d()->type === 'site' && d()->deploy_from_git) {
    drush_set_option('force_repo_path_as_toplevel', TRUE);
    drush_log(dt("Cloning site from: " . d()->repo_url), 'ok');
    provision_backend_invoke(d()->name, 'provision-git-clone');
  }
}

/**
 * Implements drush_hook_post_COMMAND().
 */
function drush_provision_git_post_provision_verify() {
  _provision_git_update_git_meta_data();
}

/**
 * Implements drush_hook_post_COMMAND() for provision-delete command.
 *
 * If repo path is different from root, delete it.
 */
function drush_provision_git_post_provision_delete() {
  // If platform has a repo path that is not the root, it must be deleted as well.
  if (d()->type =='platform' && !empty(d()->repo_path) && d()->repo_path != d()->root) {
    drush_log(dt('Deleting repo path at %path', array(
      '%path' => d()->repo_path
    )), 'ok');
    _provision_recursive_delete(d()->repo_path);
  }
}

/**
 * Update the drush context with the git_url and current git_ref.
 */
function _provision_git_update_git_meta_data() {
  if (in_array(d()->type, array('site', 'platform'))) {

    switch (d()->type) {
      case 'site':
        $repo_path = d()->site_path;
        break;

      case 'platform':
      default:
        $repo_path = d()->repo_path;
        break;
    }

    // If the repo path doesn't even contain a git config, do not update metadata.
    if (!file_exists($repo_path . '/.git/config')) {
      drush_set_option('repo_url', '');
      drush_set_option('git_ref', '');
      return;
    }

    if (drush_shell_cd_and_exec($repo_path, 'git config --local --get remote.origin.url')) {
      $repo_url = implode("\n", drush_shell_exec_output());
      drush_set_option('repo_url', $repo_url);
    }
    if (drush_shell_cd_and_exec($repo_path, 'git rev-parse --abbrev-ref HEAD')) {
      $git_ref = implode("\n", drush_shell_exec_output());
      drush_set_option('git_ref', $git_ref);
    }
  }
}


/**
 * Implements hook_provision_drupal_create_directories_alter().
 */
function provision_git_provision_drupal_create_directories_alter(&$mkdir, $url) {
  if (d()->type === 'site' && d()->deploy_from_git) {
    // Since we're cloning these directories, don't chmod/chgrp them.
    // We still need sites/$url as it gets 0555 permission otherwise [#2494113].
    unset($mkdir["sites/$url/modules"]);
    unset($mkdir["sites/$url/themes"]);
    unset($mkdir["sites/$url/libraries"]);
  }
}


/**
 * Register our directory as a place to find provision classes.
 */
function provision_git_register_autoload() {
  static $loaded = FALSE;
  if (!$loaded) {
    $loaded = TRUE;
    provision_autoload_register_prefix('Provision_', dirname(__FILE__));
  }
}

/**
 * Implements hook_drush_init().
 */
function provision_git_drush_init() {
  provision_git_register_autoload();
}

/**
 *  Implements hook_provision_services().
 */
function provision_git_provision_services() {
  provision_git_register_autoload();
  return array('git' => NULL);
}

/**
 * Load all available git hooks.
 * @return mixed
 */
function provision_git_hooks() {
  // Invoke hook_provision_git_hooks().
  $hooks = drush_command_invoke_all('provision_git_hooks');
  return $hooks;
}

/**
 * Implements hook_provision_git_hooks();
 * @return array
 */
function provision_git_provision_git_hooks() {
  return array(
    'update' => 'provision-update',
    'cache' => 'provision-flush_cache',
    'registry' => 'provision-rebuild_registry',
    'revert' => 'provision-features_revert_all',
  );
}

/**
 * Run all drush commands listed in the 'git_hooks' property of the drush alias.
 *
 * If run when a platform is active,
 */
function provision_git_run_git_hooks() {

  if (drush_get_option('skip-hooks', FALSE)) {
    drush_log('Skipping git hooks, as requested. Be aware the site might need database updates.', 'ok');
    return;
  }

  // Run post-git-pull hooks.
  $sites = array();
  if (d()->type == 'site') {
    $sites[] = d()->name;
  }
  elseif (d()->type == 'platform') {

    // Get a list of all that use this platform.
    $aliases_files = _drush_sitealias_find_alias_files();
    $aliases = array();
    foreach ($aliases_files as $filename) {
      if ((@include $filename) === FALSE) {
        drush_log(dt('Cannot open alias file "!alias", ignoring.', array('!alias' => realpath($filename))), LogLevel::BOOTSTRAP);
        continue;
      }
    }

    foreach ($aliases as $alias_name => $alias) {
      // If the alias is a site and platform is a match, load it into our sites list.
      if (isset($alias['context_type']) && $alias['context_type'] == 'site' && $alias['platform'] == d()->name) {

        // Use the URI as the key to prevent duplicate sites. Project aliases get included.
        // I'm using URI as the value as well because DevShop writes additional aliases which are copies. We want to be sure to use the Aegir-generated alias name
        $sites[$alias['uri']] = $alias['uri'];
      }
    }

    drush_log(dt('Found %num sites on this platform: %sites', array(
      '%num' => count($sites),
      '%sites' => implode(', ', $sites),
    )), 'ok');
  }

  drush_log(dt('Running git hooks for %num sites.', array(
    '%num' => count($sites),
  )), 'ok');

  $hook_commands = provision_git_hooks();
  foreach ($sites as $site) {
    // @TODO: Gotta figure out a way to put this in the Git service.
    if (is_array(d($site)->git_hooks)) {

      // Allow overriding what hooks to run on the command line.
      $hooks = drush_get_option('hooks', d($site)->git_hooks);
      if (is_string($hooks)) {
        $hooks = explode(',', $hooks);
      }
      $hooks = array_intersect($hooks, array_keys($hook_commands));

      if (empty($hooks)) {
        drush_log(dt('No hooks to invoke on site %site. Check drush context.', array(
          '%site' => $site,
        )), 'ok');
      }
      elseif (drush_get_option('hooks')) {
        drush_log(dt('hooks option detected: Running %num git hooks on site %site: %hooks.', array(
          '%num' => count($hooks),
          '%hooks' => implode(', ', $hooks),
          '%site' => $site,
        )), 'ok');
      }
      else {
        drush_log(dt('Using default git hooks for the site %site: %hooks.', array(
          '%num' => count($hooks),
          '%hooks' => implode(', ', $hooks),
          '%site' => $site,
        )), 'ok');
      }

      foreach ($hooks as $hook) {
        drush_log(dt('Invoking git hook "%hook" on site %site: %command', array(
          '%site' => $site,
          '%hook' => $hook,
          '%command' => $hook_commands[$hook],
        )), 'ok');
        provision_backend_invoke($site, $hook_commands[$hook]);
      }

      if (empty(d($site)->git_hooks)) {
        drush_log(dt('No hooks to invoke. Check drush context.'), 'ok');
      }
    }
    else {
      drush_log(dt('No hooks to invoke in site %site.', array(
        '%site' => $site,
      )), 'ok');
    }
  }
}
