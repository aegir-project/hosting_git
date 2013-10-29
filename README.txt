Provision Git
=============

Provision Git is a backend Drush module built for the AEgir Hostmaster/Provision system.
This is only for use with provision, the backend component of AEgir.
It does not provide any front-end tasks to Hostmaster.

This project is intended to be as low level as possible so that other more complex tasks can be built on top.

It provides 5 drush commands:

$ drush @alias provision-git-pull (or drush @alias pull)
$ drush @alias provision-git-push or drush @alias push)
$ drush @alias provision-git-add path/within/alias/root (or drush @alias add path/within/alias/root)
$ drush @alias provision-git-commit (or drush @alias commit)
$ drush @alias provision-git-reset (or drush @alias reset)

See drush help --filter=provision_git for details. And e.g. drush provision-git-pull --help for even more details.

Homepage: https://drupal.org/project/provision_git
