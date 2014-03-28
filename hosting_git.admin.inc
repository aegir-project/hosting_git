<?php

/**
 * @file
 * Admin UI code for Git support
 */

/**
 * General settings form.
 */
function hosting_git_settings_form() {
  $form['pullhook'] = array(
    '#type' => 'fieldset',
    '#title' => t('Pull on webhook'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );

  $form['pullhook']['hosting_git_webhook_ip_acl'] = array(
    '#type' => 'textarea',
    '#title' => t('Control Access by IP'),
    '#default_value' => variable_get('hosting_git_webhook_ip_acl', HOSTING_GIT_WEBHOOK_DEFAULT_ALLOWED_IPS),
    '#rows' => 6,
  );

  // Add some help tekst on what to fill in.
  $items = array();
  $items[] = t('Enter the IP addresses that are allowed to trigger a "Pull Code" task. You may specify address ranges using CIDR notation (e.g. 192.168.1.0/24).');
  $items[] = t('GitHub post-receive callback servers are: %github_ips.', array('%github_ips' => variable_get('hosting_git_webhook_ip_acl', HOSTING_GIT_WEBHOOK_DEFAULT_ALLOWED_IPS)));
  $items[] = t("Your local computer's IP address is %ip.", array('%ip' => ip_address()));

  // Show the last IP used to call our webhook. To make adding it easier.
  $last_ip = variable_get('hosting_git_webhook_last_ip', '');
  if ($last_ip) {
    $items[] = t('The last IP to attempt a commit notification was %ip', array('%ip' => $last_ip));
  }
  else {
    $items[] = t('No requests ever detected.  If you add the trigger URL for a project to your git repo host, the IP will be logged and displayed here.');
  }
  $form['pullhook']['hosting_git_webhook_ip_acl']['#description'] = theme('item_list', array('items' => $items));

  return system_settings_form($form);
}
