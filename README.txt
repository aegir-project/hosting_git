Provision Git
=============

Provision Git is a backend Drush module built for the AEgir Hostmaster/Provision
system.  This is only for use with provision, the backend component of AEgir.
It does not provide any front-end tasks to Hostmaster.

This project is intended to be as low level as possible so that other more
complex tasks can be built on top.

Commands
--------
It provides 5 drush commands.  Here they are along with their aliases:

    $ drush @alias provision-git-pull
    $ drush @alias pull

    $ drush @alias provision-git-push
    $ drush @alias push

    $ drush @alias provision-git-add path/within/alias/root
    $ drush @alias add path/within/alias/root

    $ drush @alias provision-git-commit
    $ drush @alias commit

    $ drush @alias provision-git-reset
    $ drush @alias reset

Compatible Modules
------------------
- Hosting Site Git: http://drupal.org/project/hosting_site_git
- DevShop: http://drupal.org/project/devshop

Help
----
See drush help --filter=provision_git for more information.
All commands have help:

    $ drush provision-git-pull --help

Source & Issues
---------------
Hosted on drupal.org: https://drupal.org/project/provision_git

Installation
------------

Assuming Drush and Provision are already installed, use drush to download:

    $ drush dl provision_git-6.x

NOTE: There is a bad release that is preventing easy download of provision git.
When installing with drush, you must explicitly add the version number, until
issue [#2123203](http://drupal.org/node/2123203) is fixed. The 7.x branch
release is old, do not use it.

