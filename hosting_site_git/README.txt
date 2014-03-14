Hosting site Git
================

This module extends drush/provision with 'git commands' in a site directory.

Supported commands are:

- git clone
- git pull (depends on provision_git)
- git checkout


Installation
------------

1. Install as any other Drupal module into your hostmaster site.
2. The file hosting_site_git.drush.inc should be copied/linked to the drush installation.
An easy way is to create a symlink in ~/.drush/ to e.g. /var/aegir/clients/admin/hostmaster.example.com/modules/hosting_site_git/.
3. Make sure you have the drush module provision_git installed (drush dl provision_git)


WARNING: If you make code modifications on the server, don't forget to commit them. They will be lost otherwise.
