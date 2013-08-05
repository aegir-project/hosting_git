<?php

/**
 * @file
 * Implements hooks for the platform/git integration.
 */

/**
 * Implementation of Implementation of hook_hosting_TASK_OBJECT_context_options().
 */
function hosting_platform_git_hosting_platform_context_options(&$task) {
  // Set some defaults
  $task->context_options['deploy_from_git'] = FALSE;
  $task->context_options['repo_branch'] = 'NOOP';

  // If we're actually provisioning from Git, populate real data.
  if (isset($task->ref->git['repo_url']) && $task->ref->git['repo_url'] != 'NOOP') {
    $task->context_options['repo_url'] = $task->ref->git['repo_url'];
    $task->context_options['deploy_from_git'] = TRUE;
    $task->context_options['repo_branch'] = $task->ref->git['repo_branch'];
  }
}

/**
 * Implementation of hook_drush_context_import().
 */
function hosting_platform_git_drush_context_import($context, &$node) {
  if ($context->type == 'platform' && $context->deploy_from_git) {
    $node->git['repo_url'] = $context->repo_url;
    $node->git['repo_branch'] = $context->repo_branch;
  }
}
