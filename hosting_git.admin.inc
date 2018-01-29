<?php

/**
 * @file
 * Admin UI code for Git support
 */

/**
 * General settings form.
 */
function hosting_git_settings_form() {
  $form = array();

  $form['hooks'] = array(
    '#type' => 'fieldset',
    '#title' => t('Git Hooks'),
  );
  $form['hooks']['hosting_git_default_hooks'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Default git hooks:'),
    '#description' => t('Choose the default hooks to use for new sites and platforms.'),
    '#options' => hosting_git_get_hook_options(),
    '#default_value' => variable_get('hosting_git_default_hooks', ['update', 'cache']),
  );
  $form['hooks']['hosting_git_allow_skip_hooks'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow users to skip git hooks when running Git Checkout or Git Pull.'),
    '#default_value' => variable_get('hosting_git_allow_skip_hooks', FALSE),
  );
  $form['hooks']['hosting_git_allow_task_override_hooks'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow users to change git hooks when running Git Checkout or Git Pull.'),
    '#default_value' => variable_get('hosting_git_allow_task_override_hooks', FALSE),
  );
  $form['sites'] = array(
    '#type' => 'fieldset',
    '#title' => t('Sites'),
  );
  $form['sites']['hosting_git_allow_deploy_site'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow deploying sites from git repositories.'),
    '#default_value' => variable_get('hosting_git_allow_deploy_site', TRUE),
  );
  $form['sites']['hosting_git_allow_hooks_site'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow sites to have configurable git hooks.'),
    '#default_value' => variable_get('hosting_git_allow_hooks_site', TRUE),
  );
  $form['platforms'] = array(
    '#type' => 'fieldset',
    '#title' => t('Platforms'),
  );
  $form['platforms']['hosting_git_allow_deploy_platform'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow deploying platforms from git repositories.'),
    '#default_value' => variable_get('hosting_git_allow_deploy_platform', TRUE),
  );
  $form['platforms']['hosting_git_allow_hooks_platform'] = array(
    '#type' => 'checkbox',
    '#title' => t('Allow platforms to have configurable git hooks.'),
    '#default_value' => variable_get('hosting_git_allow_hooks_platform', TRUE),
  );
  return system_settings_form($form);
}
