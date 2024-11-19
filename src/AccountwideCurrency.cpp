/*
 * Makes all currency shared accountwide
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"

bool AccountwideCurrencyEnabled = false;

std::vector<uint32> currencyItemIDs = {
    12840,   // Minion's Scourgestone
    12841,   // Invader's Scourgestone
    12843,   // Corruptor's Scourgestone
    19182,   // Darkmoon Faire Prize Ticket
    20558,   // Warsong Gulch Mark of Honor
    20559,   // Arathi Basin Mark of Honor
    20560,   // Alterac Valley Mark of Honor
    21229,   // Qiraji Lord's Insignia
    22637,   // Primal Hakkari Idol
    29024,   // Eye of the Storm Mark of Honor
    29434,   // Badge of Justice
    37711,   // Reward Points
    37836,   // Venture Coin
    40752,   // Emblem of Heroism
    40753,   // Emblem of Valor
    41596,   // Dalaran Jewelcrafter's Token
    42425,   // Strand of the Ancients Mark of Honor
    43016,   // Dalaran Cooking Award
    43228,   // Stone Keeper's Shard
    43307,   // Arena Points
    43308,   // Honor Points
    43589,   // Wintergrasp Mark of Honor
    43949,   // Lich Rune
    44990,   // Champion's Seal
    45624,   // Emblem of Conquest
    47241,   // Emblem of Triumph
    49426,   // Emblem of Frost
    829434,  // Badge of Glory
    829435   // Badge of Courage
};


// Load the configuration file
class AccountwideCurrencyConfig : public WorldScript
{
public:
    AccountwideCurrencyConfig() : WorldScript("AccountwideCurrencyConfig") {}

    void OnBeforeConfigLoad(bool reload) override
    {
        if (!reload)
        {
            // Load Configuration Settings
            SetInitialWorldSettings();
        }
    }
    // Load Configuration Settings
    void SetInitialWorldSettings()
    {
        AccountwideCurrencyEnabled = sConfigMgr->GetOption<bool>("AccountwideCurrency.Enable", true);
    }
};

// Load the account scripts
class AccountwideCurrencyAccount : public AccountScript {
public:
    AccountwideCurrencyAccount() : AccountScript("AccountwideCurrencyAccount") {}

    /// <summary>
    /// If the account does not yet have entries in the table, initialize them
    /// </summary>
    void OnAccountLogin(uint32 accountId) {
        InitializeAccountCurrencyOnEmptyTable(accountId);
    }

    /// <summary>
    /// If there is no table for the account, query and add the user's currencies to it.
    /// </summary>
    /// <param name="accountId"></param>
    void InitializeAccountCurrencyOnEmptyTable(uint32 accountId) {
        QueryResult checkQuery = CharacterDatabase.Query("SELECT 1 FROM accountwide_currency WHERE accountId = {} LIMIT 1", accountId);

        if (!checkQuery) {
            for (uint32 currencyId : currencyItemIDs) {
                QueryResult itemQuery;
                if (currencyId == 43308) { // Honor
                    itemQuery = CharacterDatabase.Query("SELECT SUM(totalHonorPoints) FROM characters where account = {}", accountId);
                }
                else if (currencyId == 43307) { // Arena Points
                    itemQuery = CharacterDatabase.Query("SELECT SUM(arenaPoints) FROM characters where account = {}", accountId);
                }
                else {
                    itemQuery = CharacterDatabase.Query("SELECT SUM(count) FROM item_instance WHERE itemEntry = {} AND owner_guid IN (SELECT guid FROM characters WHERE account = {})", currencyId, accountId);
                }
                if (itemQuery) {
                    uint32 count = (*itemQuery)[0].Get<uint32>();
                    UpdateAccountCurrency(accountId, currencyId, count);
                }
            }
        }
    }

    /// <summary>
    /// Update the amount of the given currency in the accountwide tabple for the account id
    /// </summary>
    /// <param name="accountId"></param>
    /// <param name="currencyId"></param>
    /// <param name="newCount"></param>
    void UpdateAccountCurrency(uint32 accountId, uint32 currencyId, uint32 newCount) {
        CharacterDatabase.Execute("INSERT INTO accountwide_currency (accountId, currencyId, count) VALUES ({}, {}, {}) ON DUPLICATE KEY UPDATE count = {}", accountId, currencyId, newCount, newCount);
    }
};


// Load the player scripts

class AccountwideCurrency : public PlayerScript
{
public:
    AccountwideCurrency() : PlayerScript("AccountwideCurrency") {}

    void OnLogin(Player* player) //override
    {
        uint32 accountId = player->GetSession()->GetAccountId();
        SyncCurrencyOnLogin(player, accountId);
    }

    //void OnLogout(Player* player) //override
    //{
    //    MatchSavedData(player);
    //}

    void OnSave(Player* player) //override
    {
        MatchSavedData(player);
    }

    /// <summary>
    /// Uploads the currency amounts in the player's account table
    /// </summary>
    /// <param name="player"></param>
    void MatchSavedData(Player* player) {
        uint32 accountId = player->GetSession()->GetAccountId();
        for (uint32 currencyId : currencyItemIDs) {
            std::cout << "Currency ID: " << currencyId << std::endl;

            uint32 accountCurrencyCount = FetchAccountCurrency(accountId, currencyId);
            uint32 playerCurrencyCount = 0;

            if (currencyId == 43308) { // honor
                playerCurrencyCount = player->GetHonorPoints();
            }
            else if (currencyId == 43307) { // arena
                playerCurrencyCount = player->GetArenaPoints();
            }
            else {
                playerCurrencyCount = player->GetItemCount(currencyId);
            }

            if (accountCurrencyCount != playerCurrencyCount) {
                UpdateAccountCurrency(accountId, currencyId, playerCurrencyCount);
            }
        }
    }

    /// <summary>
    /// Retrieve the quantity of the given currency from the accountwide table for the account id
    /// </summary>
    /// <param name="accountId"></param>
    /// <param name="currencyId"></param>
    /// <returns></returns>
    uint32 FetchAccountCurrency(uint32 accountId, uint32 currencyId) {
        QueryResult query = CharacterDatabase.Query("SELECT count FROM accountwide_currency WHERE accountId = {} AND currencyId = {}", accountId, currencyId);
        if (query) {
            return (*query)[0].Get<uint32>();
        }
        return 0;
    }

    /// <summary>
    /// Update the amount of the given currency in the accountwide tabple for the account id
    /// </summary>
    /// <param name="accountId"></param>
    /// <param name="currencyId"></param>
    /// <param name="newCount"></param>
    void UpdateAccountCurrency(uint32 accountId, uint32 currencyId, uint32 newCount) {
        CharacterDatabase.Execute("INSERT INTO accountwide_currency (accountId, currencyId, count) VALUES ({}, {}, {}) ON DUPLICATE KEY UPDATE count = {}", accountId, currencyId, newCount, newCount);
    }

    /// <summary>
    /// Updates the characters currency to match the account table
    /// </summary>
    /// <param name="player"></param>
    /// <param name="accountId"></param>
    void SyncCurrencyOnLogin(Player* player, uint32 accountId) {
        for (uint32 currencyId : currencyItemIDs) {
            std::cout << "Currency ID: " << currencyId << std::endl;

            uint32 accountCurrencyCount = FetchAccountCurrency(accountId, currencyId);
            uint32 playerCurrencyCount = 0;

            if (currencyId == 43308) { // Honor
                playerCurrencyCount = player->GetHonorPoints();
            }
            else if (currencyId == 43307) { // Arena
                playerCurrencyCount = player->GetArenaPoints();
            }
            else {
                playerCurrencyCount = player->GetItemCount(currencyId);
            }

            if (playerCurrencyCount < accountCurrencyCount) {
                if (currencyId == 43308) { // Honor
                    player->ModifyHonorPoints(accountCurrencyCount - playerCurrencyCount);
                }
                else if (currencyId == 43307) { // Arena
                    player->ModifyArenaPoints(accountCurrencyCount - playerCurrencyCount);
                }
                else {
                    player->AddItem(currencyId, accountCurrencyCount - playerCurrencyCount);
                }
            }
            else if (playerCurrencyCount > accountCurrencyCount) {
                if (currencyId == 43308) { // Honor
                    player->ModifyHonorPoints(-1 * playerCurrencyCount);
                }
                else if (currencyId == 43307) { // Arena
                    player->ModifyArenaPoints(-1 * playerCurrencyCount);
                }
                else {
                    player->DestroyItemCount(currencyId, playerCurrencyCount, true, true);
                }
            }
        }
    }
};

// Add all scripts in one
void AddAccountwideCurrencyScripts()
{
    new AccountwideCurrencyConfig();
    new AccountwideCurrencyAccount();
    new AccountwideCurrency();
}
