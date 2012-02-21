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
  $items['provision-git_pull'] = array(
    'description' => 'Git pull a site directory.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH
  );

  $items['provision-git_clone'] = array(
    'description' => 'Git clone a repo in a specified location.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH
  );
  
  $items['provision-git_checkout'] = array(
    'description' => 'Git checkout a tag in a specified location.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH
  );
  
  return $items;
}

/**
 * Implements the provision-git_pull command.
 */
function drush_hosting_site_git_provision_git_pull() {
  $site_path = d()->site_path;

  // Check that the site is actually using Git
  if (!is_dir($site_path . '/.git')) {
    return drush_set_error('DRUSH_PROVISION_GIT_PULL_FAILED', dt("The site does not seem to be a valid Git clone in !path", array('!path' => $site_path)));
  }

  // Reset local modifications, git pull fails when there are local modifications in Git, wipe those first DANGEROUS!
  drush_shell_cd_and_exec($site_path, 'git reset --hard');

  // Pull
  if (drush_shell_cd_and_exec($site_path, 'git pull')) {
    drush_log(dt('Pulled from git on !path', array('!path' => $site_path)), 'ok');

    // Submodule in git? lets update those.
    if (file_exists($site_path . '/.gitmodules')) {
      drush_shell_cd_and_exec($site_path, 'git submodule init');
      drush_shell_cd_and_exec($site_path, 'git submodule update');
    } 

    // Re-verify the site, this corrects the file permission when necessary
    $options = array();
    $target = d()->uri;
    provision_backend_invoke($target, 'provision-verify', array(), $options);
  }
  else {
    return drush_set_error('DRUSH_PROVISION_GIT_PULL_FAILED', dt("Drush was unable to get Git pull in the site dir on !path.\nThe specific errors are below:\n!errors", array('!path' => $site_path, '!errors' => implode("\n", drush_shell_exec_output()))));
  }
}

/**
 * Implements the provision-git_clone command
 */
function drush_hosting_site_git_provision_git_clone() {
	drush_log(dt('STUB'), 'ok');
	$task =& drush_get_context('HOSTING_TASK');
	
/*	$location = drush_get_option('location');
	$gitrepo = drush_get_option('gitrepo');
	
	
	drush_log(dt('Location: ').$location, 'ok');
	drush_log(dt('Git repo: ').$gitrepo, 'ok');
	*/
}

/**
 * Implements the provision-git_checkout command 
 */
function drush_hosting_site_git_provision_git_checkout() {
	drush_log(dt('STUB'), 'ok');
}