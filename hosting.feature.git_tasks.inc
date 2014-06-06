<?php

/**
 * @file
 *   The hosting feature definition for hosting_git.
 */

/**
 * Register a hosting feature with Aegir.
 *
 * This will be used to generate the 'admin/hosting' page.
 *
 * @return
 *  associative array indexed by feature key.
 */
function hosting_git_hosting_feature() {
  $features['git_pull'] = array(
    // title to display in form
    'title' => t('Git pull tasks'),
    // description
    'description' => t('Enables git pulls on sites and platforms.'),
    // initial status ( HOSTING_FEATURE_DISABLED, HOSTING_FEATURE_ENABLED, HOSTING_FEATURE_REQUIRED )
    'status' => HOSTING_FEATURE_DISABLED,
    // module to enable/disable alongside feature
    'module' => 'hosting_git',
    // associate with a specific node type.
    //  'node' => 'nodetype',
    // which group to display in ( null , experimental )
    'group' => 'experimental',
  );
  return $features;
}
