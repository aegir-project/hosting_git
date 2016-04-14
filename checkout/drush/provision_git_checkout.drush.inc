<?php

/**
 * Implements drush_hook_post_COMMAND().
 */
function drush_provision_git_checkout_post_provision_git_checkout() {
  // Re-verify the site, this corrects the file permission when necessary.
  $options = array();
  $target = d()->uri;
  provision_backend_invoke($target, 'provision-verify', array(), $options);
}
