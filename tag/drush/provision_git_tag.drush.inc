<?php

/**
 * Implementation of hook_drush_command().
 */
function provision_git_tag_drush_command() {
  $items['provision-git-tag'] = array(
    'description' => 'Create a new git tag.',
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'arguments' => array(
      'git_tag' => dt('The desired name of the tag.'),
    ),
    'options' => array(
      'message' => 'The message to attach to the tag.',
      'checkout' => "When done creating the tag, check it out so the repository stays on that tag.",
      'push' => "Push the tag to the repository remote 'origin'.",
    )
  );
  return $items;
}

/**
 * Pre provision-git-tag hook
 */
function drush_provision_pre_provision_git_tag($git_tag = '') {
  provision_git_is_repo();
}

/**
 * Implements the provision-git_tag command
 */
function drush_provision_git_tag($git_tag = '') {

  // Get URL and path from d()
  if (d()->type == 'site') {
    $path = d()->site_path;
  }
  else {
    $path = d()->root;
  }

  // Make sure there is a git ref
  if (empty($git_tag)) {
    return drush_set_error('DRUSH_PROVISION_GIT_TAG_FAILED', dt("Git tag failed: Must specify a tag name."));
  }

  $command = "git tag " . escapeshellarg(trim($git_tag));
  if (drush_get_option('message', '')) {
    $command .= ' -m ' . escapeshellarg(drush_get_option('message'));
  }

  // Try to perform the git checkout.
  drush_log("Running $command", 'ok');
  if (drush_shell_cd_and_exec($path, $command)) {
    // Log the fact that it worked.
    drush_log(dt('Git tag created: %tag.', array('%tag' => $git_tag)), 'ok');

    // If 'checkout' was specified, checkout the tag.
    if (drush_get_option('checkout', FALSE)) {
      if (drush_shell_cd_and_exec($path, "git checkout $git_tag")) {
        drush_log(dt('Checked out git tag: %output', array('%output' => implode("\n", drush_shell_exec_output()))), 'ok');
      }
      else {
        drush_log(dt('Unable to check out git tag: %output', array('%output' => implode("\n", drush_shell_exec_output()))), 'warning');
      }
    }

    // If 'push' was specified, run git push.
    if (drush_get_option('push', FALSE)) {
      if (drush_shell_cd_and_exec($path, "git push origin $git_tag")) {
        drush_log(dt('Git tag pushed to remote: %output', array('%output' => implode("\n", drush_shell_exec_output()))), 'ok');
      }
      else {
        drush_log(dt('Unable to push tag to remote: %output', array('%output' => implode("\n", drush_shell_exec_output()))), 'warning');
      }
    }

    _provision_git_update_git_meta_data();
  }
  else {
    // The command failed.
    return drush_set_error('DRUSH_PROVISION_GIT_CHECKOUT_FAILED', dt("Git tag failed! \nThe specific errors are below:\n!errors", array('!errors' => implode("\n", drush_shell_exec_output()))));
  }
}
