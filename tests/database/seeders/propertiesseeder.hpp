#pragma once

#include <tom/seeder.hpp>

namespace Seeders
{

    struct PropertiesSeeder : Seeder
    {
        /*! Run the database seeders. */
        void run() override
        {
            DB::table("properties")->insert({
                {{"name", "1. property"}},
                {{"name", "2. property"}},
            });
        }
    };

} // namespace Seeders
