<?php

/**
 * @file
 * Provision/Drush hooks for the provision-git_pull command.
 *
 * These are the hooks that will be executed by the drush_invoke function
 * when asking for a git pull in the site specific directory.
 */

/**
 * Map values of site node into command line arguments.
 */
function drush_hosting_git_pre_hosting_task($task) {
  $task = &drush_get_context('HOSTING_TASK');
  if ($task->task_type == 'git-clone') {
    if (!empty($task->task_args['git_url'])) {
      // Pass the argument provision_git expects.
      $task->args[0] = $task->task_args['git_url'];
    }
    if (!empty($task->task_args['path'])) {
      // Pass the argument provision_git expects.
      $task->args[1] = $task->task_args['path'];
    }
    if (!empty($task->task_args['git_ref'])) {
      // Pass the option provision_git expects.
      $task->args[2] = $task->task_args['git_ref'];
    }
  }

  // Only work on selected task types.
  if (!in_array($task->task_type, array('git-pull', 'git-checkout'))) {
    return;
  }

  if (!empty($task->task_args)) {
    if ($task->task_args['reset_hard'] == TRUE) {
      // Pass the option provision_git expects.
      $task->options['reset'] = TRUE;
    }
    if (!empty($task->task_args['checkout_git_ref'])) {
      // Pass the option provision_git expects.
      $task->options['git_ref'] = $task->task_args['checkout_git_ref'];
    }
  }

  // --skip-hooks option.
  $task->options['skip-hooks'] = $task->task_args['skip_hooks'];

  // --commands option. We pass a list of drush commands so we don't need a duplicate hook_hosting_git_hooks() in provision.
  if (!empty($task->task_args['git_hooks'])) {
    $task->options['hooks'] = $task->task_args['git_hooks'];
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
  $task->context_options['git_ref'] = '';

  // If we're actually provisioning from Git, populate real data.
  if (!empty($task->ref->git['repo_url'])) {
    $task->context_options['repo_url'] = $task->ref->git['repo_url'];
    $task->context_options['repo_path'] = $task->ref->git['repo_path'];
    $task->context_options['deploy_from_git'] = TRUE;
    $task->context_options['git_ref'] = $task->ref->git['git_ref'];

    // Transform the list of selected hooks to pass the command to the context_option.
    foreach (array_filter($task->ref->git['git_hooks']) as $hook_name) {
      $task->context_options['git_hooks'][] = $hook_name;
    }
  }
}

/**
 * Implements hook_hosting_TASK_OBJECT_context_options().
 */
function hosting_git_hosting_site_context_options(&$task) {
  hosting_git_hosting_platform_context_options($task);
}

/**
 * Implements hook_drush_context_import().
 */
function hosting_git_drush_context_import($context, &$node) {
  if (in_array($context->type, array('site', 'platform')) && $context->deploy_from_git) {
    $node->git['repo_url'] = $context->repo_url;
    $node->git['git_ref'] = $context->git_ref;
    $node->git['repo_path'] = $context->repo_path;
    $node->git['git_hooks'] = $context->git_hooks;
  }
}

/**
 * Implements hook_post_hosting_TASK_TYPE_task().
 */
function hosting_git_post_hosting_verify_task($task, $data) {
  $node = $task->ref;
  $context = $data['context'];
  if ($context['deploy_from_git']) {
    if (isset($context['repo_url'])) {
      $node->git['repo_url'] = $context['repo_url'];
    }

    if (isset($context['git_ref'])) {
      $node->git['git_ref'] = $context['git_ref'];
    }

    if (isset($context['repo_path'])) {
      $node->git['repo_path'] = $context['repo_path'];
    }
  }
}

/**
 * Implements hook_post_hosting_TASK_TYPE_task().
 *
 * When git checkout occurs, code may change, so run verify tasks.
 */
function hosting_git_post_hosting_git_checkout_task($task, $data) {
  hosting_git_run_verify_tasks($task);
}

/**
 * Implements hook_post_hosting_TASK_TYPE_task().
 *
 * When git pull occurs, code may change, so run verify tasks.
 */
function hosting_git_post_hosting_git_pull_task($task, $data) {
  hosting_git_run_verify_tasks($task);
}

/**
 * Helper for post-checkout and post-pull tasks to trigger verify tasks for sites and platform.
 * @param $task
 */
function hosting_git_run_verify_tasks($task){
  if (isset($task->ref->nid)) {
    hosting_add_task($task->ref->nid, 'verify');
    drush_log(dt('Verify task queued for %name', array(
      '%name' => $task->ref->title,
    )), 'ok');

    // If task was on a site, queue up a verify for it's platform.
    if ($task->ref->type == 'site') {
      hosting_add_task($task->ref->platform, 'verify');
      drush_log(dt('Verify task queued for platform %nid', array(
        '%nid' => $task->ref->platform,
      )), 'ok');
    }
    // If task was on a platform, queue up a verify for all sites on the platform.
    elseif ($task->ref->type == 'platform') {

      $sites = db_select('hosting_site', 's')
        ->fields('s', array('nid'))
        ->condition('platform', $task->ref->nid)
        ->condition('s.status', HOSTING_SITE_ENABLED, '=')
        ->execute()
        ->fetchAllKeyed(0, 0);

      foreach ($sites as $site) {
        hosting_add_task($site, 'verify');
        drush_log(dt('Verify task queued for site %nid', array(
          '%nid' => $site,
        )), 'ok');
      }
    }
  }
}
