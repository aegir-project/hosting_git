<?php

/**
 * @file
 * Provision hooks for the hosting_site_git_pull command.
 *
 * These are the hooks that will be executed by the drush_invoke function
 * when asking for a git pull in the site specific directory.
 */

/**
 * Implementation of hook_drush_command().
 */
function hosting_site_git_pull_drush_command() {
  $items['provision-hosting_site_git_pull'] = array(
    'description' => 'Git pull a site directory.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH
  );
  drush_log('dsaf', 'debug');
  return $items;
}


/**
 * 
 */
function drush_hosting_site_git_pull_provision_hosting_site_git_pull() {
  $site_path = d()->site_path;
  drush_log('Entering drush_provis****ion_hosting_site_git_pull', 'ok');
drush_log(print_r(d(), 1), 'ok');
  // Check that the site is actually using Git
  if (!is_dir($site_path . '/.git')) {
    return drush_set_error('DRUSH_PROVISION_GIT_PULL_FAILED', dt("The site does not seem to ge a valid Git clone"));
  }

  if (drush_shell_cd_and_exec($site_path, 'git pull')) {
    drush_log(dt('Pulled from git on !path', array('!path' => $site_path)), 'ok');
  }
  else {
    return drush_set_error('DRUSH_PROVISION_GIT_PULL_FAILED', dt("Drush was unable to get Git pull in the site dir on !path.\nThe specific errors are below:\n!errors", array('!path' => $site_path, '!errors' => implode("\n", drush_shell_exec_output()))));
  }
}
function drush_provision_hosting_site_git_pull() {
  $site_path = d()->site_path;
drush_log('Entering drush_provision_hosting_site_git_pull', 'ok');
  // Check that the site is actually using Git
  if (!is_dir($site_path . '/.git')) {
    return drush_set_error('DRUSH_PROVISION_GIT_PULL_FAILED', dt("The site does not seem to ge a valid Git clone"));
  }

  if (drush_shell_cd_and_exec($site_path, 'git pull')) {
    drush_log(dt('Pulled from git on !path', array('!path' => $site_path)), 'ok');
  }
  else {
    return drush_set_error('DRUSH_PROVISION_GIT_PULL_FAILED', dt("Drush was unable to get Git pull in the site dir on !path.\nThe specific errors are below:\n!errors", array('!path' => $site_path, '!errors' => implode("\n", drush_shell_exec_output()))));
  }
}
