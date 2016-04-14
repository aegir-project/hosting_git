<?php

/**
 * Implementation of hook_drush_command().
 */
function provision_git_checkout_drush_command() {
  $items['provision-git-checkout'] = array(
    'description' => 'Git checkout a branch or tag in a specified location.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'arguments' => array(
      'git_ref' => dt('Parameter for the git checkout command'),
    ),
    'options' => array(
      'new_branch' => "Optional: If the branch doesn't exist, create it. Equivalent to git checkout -b branchname"
    )
  );
  return $items;
}

/**
 * Pre provision-git-checkout hook
 */
function drush_provision_pre_provision_git_checkout() {
  provision_git_is_repo();
}

/**
 * Implements the provision-git_checkout command
 */
function drush_provision_git_checkout($git_ref = '') {

  // Get URL and path from d()
  // @TODO: Move to a validate hook.
  if (!$git_ref) {
    $git_ref = d()->git_ref;
  }
  if (d()->type == 'site') {
    $path = d()->site_path;
  }
  else {
    $path = d()->root;
  }

  // Make sure there is a git ref
  if (empty($git_ref)) {
    return drush_set_error('DRUSH_PROVISION_GIT_CHECKOUT_FAILED', dt("Git checkout failed: No git_ref specified."));
  }

  provision_backend_invoke(d()->name, 'provision-git-fetch', array(), array('repo_path' => $path));

  drush_log(dt('Checking out git reference !git_ref', array('!git_ref' => $git_ref), 'ok'));

  $command = "git checkout " . escapeshellarg(trim($git_ref));

  // Try to perform the git checkout.
  drush_log("Running $command", 'ok');
  if (drush_shell_cd_and_exec($path, $command)) {
    // Log the fact that it worked.
    drush_log(dt('Checkout successful.'), 'ok');
    drush_log(implode("\n", drush_shell_exec_output()), 'ok');

    _provision_git_update_git_meta_data();
  } else {
    // The command failed.
    return drush_set_error('DRUSH_PROVISION_GIT_CHECKOUT_FAILED', dt("Git checkout failed! \nThe specific errors are below:\n!errors", array('!errors' => implode("\n", drush_shell_exec_output()))));
  }
}

/**
 * Implements drush_hook_post_COMMAND().
 */
function drush_provision_git_checkout_post_provision_git_checkout() {
  // Re-verify the site, this corrects the file permission when necessary.
  $options = array();
  $target = d()->uri;
  provision_backend_invoke($target, 'provision-verify', array(), $options);
}
