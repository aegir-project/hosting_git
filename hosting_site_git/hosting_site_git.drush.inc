<?php

/**
 * @file
 * Provision/Drush hooks for the provision-git_pull command.
 *
 * These are the hooks that will be executed by the drush_invoke function
 * when asking for a git pull in the site specific directory.
 */

/**
 * Implementation of hook_drush_command().
 */
function hosting_site_git_drush_command() {
  // The provision-git_pull command has been replace with provision-git-pull from provision_git

  $items['provision-git_clone'] = array(
    'description' => 'Git clone a repo in a specified location.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'arguments' => array(
      'gitrepo' => dt('The git repository to clone.'),
      'location' => dt('The location where the git repository will be cloned.'),
    ),
  );

  $items['provision-git_checkout'] = array(
    'description' => 'Git checkout a branch or tag in a specified location.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'arguments' => array(
      'tag' => dt('Parameter for the git checkout command'),
      'location' => dt('Location in the site structure where the command should be performed.')
    ),
  );

  return $items;
}

/**
 * Implements drush_hook_post_COMMAND().
 */
function drush_hosting_site_git_post_provision_git_pull() {
    // Re-verify the site, this corrects the file permission when necessary
    $options = array();
    $target = d()->uri;
    provision_backend_invoke($target, 'provision-verify', array(), $options);
}

/**
 * Map values of site node into command line arguments
 */
function drush_hosting_site_git_pre_hosting_task($task) {
  $task =& drush_get_context('HOSTING_TASK');

  // Only work on selected task types.
  if (!in_array($task->task_type, array('git-pull', 'git_clone', 'git_checkout'))) {
    return;
  }

  if (!empty($task->task_args) && $task->task_args['reset_hard'] == TRUE) {
    // Pass the option provision_git expects.
    $task->options['reset'] = TRUE;
  }

  // Force the repository path, provision_git could otherwise look up the tree and find a platform dir under git control.
  $task->options['force_repo_path_as_toplevel'] = TRUE;


  // Pass the dialog entries to the drush provision script
  if ($task->task_type == 'git_clone') {
    $task->args[1] = $task->task_args['gitrepo'];
    $task->args[2] = $task->task_args['location'];
  }
  elseif ($task->task_type == 'git_checkout') {
    $task->args[1] = $task->task_args['tag'];
    $task->args[2] = $task->task_args['location'];
  }
}

/**
 * Implements the provision-git_clone command
 */
function drush_hosting_site_git_provision_git_clone($gitrepo, $location) {

  drush_log(dt('Start git clone command'), 'ok');
  drush_log(dt('Git repo: ') . $gitrepo, 'ok');
  drush_log(dt('Location: ') . $location, 'ok');

  // Get the site path
  $site_path = d()->site_path;

  // Location to execute the git command
  // TODO Check if dir exists, if not create it

  $git_command_path = $site_path . '/' . $location;

  // Try to perform the git clone
  if (drush_shell_cd_and_exec($git_command_path, 'git clone %s .', $gitrepo)) {
    // Log the fact that it worked
    drush_log(dt('Cloned from git on !path', array('!path' => $git_command_path)), 'ok');
    drush_log(dt('!output', array('!output' => implode("\n", drush_shell_exec_output()))), 'notice');

    // Verify the site to include any changes
    $options = array();
    $target = d()->uri;
    provision_backend_invoke($target, 'provision-verify', array(), $options);
  }
  else {
    // The command failed
    return drush_set_error('DRUSH_PROVISION_GIT_CLONE_FAILED', dt("Drush was unable to get Git clone in the site dir on !path.\nThe specific errors are below:\n!errors", array('!path' => $git_command_path, '!errors' => implode("\n", drush_shell_exec_output()))));
  }
}

/**
 * Implements the provision-git_checkout command
 */
function drush_hosting_site_git_provision_git_checkout($tag, $location) {
  drush_log(dt('Start git checkout command'), 'ok');
  drush_log(dt('Git tag: ') . $tag, 'ok');
  drush_log(dt('Location: ') . $location, 'ok');

  // Get the site path
  $site_path = d()->site_path;

  // Location to execute the git command
  // TODO Check if dir exists, if not throw an error
  $git_command_path = $site_path . '/' . $location;

  // Fetch the tags
  if (drush_shell_cd_and_exec($git_command_path, 'git fetch --tags')) {
    drush_log(dt('Git fetch command executed.'), 'ok');

    // Perform a checkout of the selected tag
    if (drush_shell_cd_and_exec($git_command_path, 'git checkout %s', $tag)) {
      drush_log(dt('Git checkout command executed.'), 'ok');
      // Log the output
      drush_log(dt('!output', array('!output' => implode("\n", drush_shell_exec_output()))), 'notice');

      // Verify the site to include any changes
      $options = array();
      $target = d()->uri;
      provision_backend_invoke($target, 'provision-verify', array(), $options);
    }
    else {
      return drush_set_error('DRUSH_PROVISION_GIT_CHECKOUT_FAILED', dt("Drush was unable to perform a git checkout !tag on !path.\nThe specific errors are below:\n!errors", array('!path' => $git_command_path, '!tag' => $tag, '!errors' => implode("\n", drush_shell_exec_output()))));
    }
  }
  else {
    return drush_set_error('DRUSH_PROVISION_GIT_FETCH_FAILED', dt("Drush was unable to perform a git fetch --tags on !path.\nThe specific errors are below:\n!errors", array('!path' => $git_command_path, '!errors' => implode("\n", drush_shell_exec_output()))));
  }
}