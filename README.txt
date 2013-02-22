Hosting site Git
=====================

This module extends drush/provision with 'git commands' in a site directory.

Supported commands are:

- git clone
- git pull
- git checkout


INSTALL
=================

The file hosting_site_git.drush.inc should be copied to the drush installation.
As it has code for both hostmaster and drush it needs to be in both a modules directory of hostmaster and a directory where drush looks e.g. ~/.drush/.


WARNING: If you make code modifications on the server, don't forget to commit them. They will be lost otherwise.
