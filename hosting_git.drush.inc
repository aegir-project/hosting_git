<?php

/**
 * @file
 * Provision/Drush hooks for the provision-git_pull command.
 *
 * These are the hooks that will be executed by the drush_invoke function
 * when asking for a git pull in the site specific directory.
 */

/**
 * Implements drush_hook_post_COMMAND().
 */
function drush_hosting_git_post_provision_git_pull() {
  // Re-verify the site, this corrects the file permission when necessary.
  $options = array();
  $target = d()->uri;
  provision_backend_invoke($target, 'provision-verify', array(), $options);
}

/**
 * Map values of site node into command line arguments.
 */
function drush_hosting_git_pre_hosting_task($task) {
  $task = &drush_get_context('HOSTING_TASK');

  // Only work on selected task types.
  if (!in_array($task->task_type, array('git-pull', 'git_clone', 'git_checkout'))) {
    return;
  }

  if (!empty($task->task_args) && $task->task_args['reset_hard'] == TRUE) {
    // Pass the option provision_git expects.
    $task->options['reset'] = TRUE;
  }

  // Force the repository path.
  // Otherwise git climbs the tree to find a platform dir under git control.
  $task->options['force_repo_path_as_toplevel'] = TRUE;
}

/**
 * Implements hook_hosting_TASK_OBJECT_context_options().
 */
function hosting_git_hosting_platform_context_options(&$task) {
  // Set some defaults.
  $task->context_options['deploy_from_git'] = FALSE;
  $task->context_options['repo_branch'] = '';

  // If we're actually provisioning from Git, populate real data.
  if (!empty($task->ref->git['repo_url'])) {
    $task->context_options['repo_url'] = $task->ref->git['repo_url'];
    $task->context_options['deploy_from_git'] = TRUE;
    $task->context_options['git_ref'] = $task->ref->git['git_ref'];
  }
}

/**
 * Implements hook_drush_context_import().
 */
function hosting_git_drush_context_import($context, &$node) {
  if ($context->type == 'platform' && $context->deploy_from_git) {
    $node->git['repo_url'] = $context->repo_url;
    $node->git['git_ref'] = $context->git_ref;
  }
}
