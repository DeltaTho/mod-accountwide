# Accountwide Currency Sharing

[English](README.md) | [Español](README_ES.md)


## What it Does

-- Makes all itemized currencies shared across the account.
-- Makes Honor and Arena points shared across the account.


## How to Install / How it does it

Simply clone into the modules directory.  A Sql script will add the table to the characters database.  The first login on the account will populate the currencies of all characters for the user.  When a character logs in, they will by synced with the accountwide table.  The table will be updated whenever the character is saved.
