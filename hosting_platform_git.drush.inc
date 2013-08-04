<?php

/**
 * @file
 * Implements hooks for the platform/git integration.
 */

function hosting_hosting_platform_context_options(&$task) {
  $task->context_options['deploy_from_git'] = FALSE;
  if (isset($task->ref->git['repo_url'] != 'NOOP')) {
    $task->context_options['deploy_from_git'] = TRUE;
    $task->context_options['repo_url'] = $task->ref->git['repo_url'];
    $task->context_options['repo_branch'] = $task->ref->git['repo_branch'];
  }
}
