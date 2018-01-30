<?php
/**
 * @file
 * Hooks provided by the hosting_git module.
 */

/**
 * Define hooks for running after code changes by Git.
 *
 * @return array
 */
function hook_hosting_git_hooks() {
  return array(
    'echo' => array(
      'description' => t('Echo hello world'),
    )
  );
}

/**
 * Advertise what hooks are available to be called after a git pull or checkout.
 *
 * @return
 *   An associative array of type => drush command.
 *
 * @see provision.service.inc
 */
function hook_provision_git_hooks() {
  return array(
    'update' => 'provision-update',
  );
}

