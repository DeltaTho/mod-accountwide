# Accountwide Currency Sharing

[English](README.md)

Inspired by https://github.com/Aldori15/azerothcore-eluna-accountwide
-Removes Eluna / Lua dependency by working with pure C++
-Now works for multiple accounts on the server
-Simpler event structure, prevents race conditions
-Can easily be expanded to support heirlooms, mounts, gold, etc.

## What it Does

-- Makes all itemized currencies shared across the account.
-- Makes Honor and Arena points shared across the account.


## How to Install / How it does it work

Simply clone into the modules directory.  A Sql script will add the table to the characters database.  The first login on the account will populate the currencies of all characters for the user.  When a character logs in, they will by synced with the accountwide table.  The table will be updated whenever the character is saved.
