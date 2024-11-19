-- Ensure we are using the characters database
USE `characters`;

-- Create the accountwide_currency table if it doesn't already exist
CREATE TABLE IF NOT EXISTS `accountwide_currency` (
    `accountId` INT UNSIGNED NOT NULL,
    `currencyId` INT UNSIGNED NOT NULL,
    `count` INT NOT NULL,
    PRIMARY KEY (`accountId`, `currencyId`)
);

-- Create the accountwide_mounts table if it doesn't already exist
-- CREATE TABLE IF NOT EXISTS `accountwide_mounts` (
--     `accountId` INT UNSIGNED NOT NULL,
--     `mountSpellId` INT UNSIGNED NOT NULL,
--     PRIMARY KEY (`accountId`, `mountSpellId`)
-- );
