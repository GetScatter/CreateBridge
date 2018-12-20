#pragma once

namespace accounts {

    struct permission_level_weight {
        permission_level  permission;
        uint16_t          weight;
    };

    struct key_weight {
        eosio::public_key  key;
        uint16_t           weight;
    };

    struct wait_weight {
        uint32_t           wait_sec;
        uint16_t           weight;
    };

    struct authority {
        uint32_t                              threshold = 0;
        std::vector<key_weight>               keys;
        std::vector<permission_level_weight>  accounts;
        std::vector<wait_weight>              waits;
    };

    struct newaccount {
        name creator;
        name name;
        authority owner;
        authority active;
    };
}
