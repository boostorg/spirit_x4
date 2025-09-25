/*=============================================================================
    Copyright (c) 2001-2015 Joel de Guzman
    Copyright (c) 2025 Nana Sakisaka

    Distributed under the Boost Software License, Version 1.0. (See accompanying
    file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#include "test.hpp"

#include <boost/spirit/x4/auxiliary/attr.hpp>
#include <boost/spirit/x4/auxiliary/eps.hpp>
#include <boost/spirit/x4/char/char.hpp>
#include <boost/spirit/x4/char/negated_char.hpp>
#include <boost/spirit/x4/string/string.hpp>
#include <boost/spirit/x4/operator/sequence.hpp>
#include <boost/spirit/x4/operator/plus.hpp>
#include <boost/spirit/x4/operator/list.hpp>
#include <boost/spirit/x4/operator/alternative.hpp>

#include <boost/fusion/include/at_key.hpp>
#include <boost/fusion/include/make_map.hpp>
#include <boost/fusion/adapted/struct.hpp>

#include <boost/variant.hpp>

#include <string>

struct AdaptedStruct {
    std::string key1;
    std::string key2;
};

class key1_attr {};
class key2_attr {};

BOOST_FUSION_ADAPT_ASSOC_STRUCT(
    AdaptedStruct,
    (std::string, key1, class key1_attr)
    (std::string, key2, class key2_attr)
)

TEST_CASE("fusion_map")
{
    using x4::lit;
    using x4::attr;
    using x4::char_;
    using x4::eps;
    namespace fusion = boost::fusion;

    {
        // parsing sequence directly into fusion map
        constexpr auto key1 = lit("key1") >> attr(key1_attr());
        constexpr auto kv1 = key1 >> lit("=") >> +char_;

        {
           auto attr_ =  fusion::make_map<key1_attr>(std::string());
           REQUIRE(parse("key1=ABC", kv1, attr_));
           CHECK(fusion::at_key<key1_attr>(attr_) == "ABC");
        }
        {
           AdaptedStruct attr_;
           REQUIRE(parse("key1=ABC", kv1, attr_));
           CHECK(attr_.key1 == "ABC");
           CHECK(attr_.key2 == "");
        }
    }
    {   // case when parser handling fusion assoc sequence
        // is on one side of another sequence
        constexpr auto key1 = lit("key1") >> attr(key1_attr());
        constexpr auto kv1 = key1 >> lit("=") >> +~char_(';');

        AdaptedStruct attr_;
        REQUIRE(parse("key1=ABC", eps >>  (kv1 % ';') , attr_));
        CHECK(attr_.key1 == "ABC");
        CHECK(attr_.key2 == "");
    }
    {
        // parsing repeated sequence directly into fusion map (overwrite)
        constexpr auto key1 = lit("key1") >> attr(key1_attr());
        constexpr auto kv1 = key1 >> lit("=") >> +~char_(';');

        {
            auto attr_ =  fusion::make_map<key1_attr>(std::string());
            constexpr auto parser = kv1 % ';';
            REQUIRE(parse("key1=ABC;key1=XYZ", parser, attr_));
            CHECK(fusion::at_key<key1_attr>(attr_) == "XYZ");
        }
        {
            AdaptedStruct attr_;
            REQUIRE(parse("key1=ABC;key1=XYZ", kv1 % ';', attr_));
            CHECK(attr_.key1 == "XYZ");
        }
    }

    {   // parsing repeated sequence directly into fusion map (append)

        /* NOT IMPLEMENTED
        auto const key1 = lit("key1") >> attr(key1_attr());
        auto const kv1 = key1 >> lit("=") >> +char_;
        auto attr_ =  fusion::make_map<key1_attr>(std::vector<std::string>());

        CHECK(parse("key1=ABC;key1=XYZ", kv1 % ";", attr_));
        CHECK(fusion::at_key<key1_attr>(attr_) == {"ABC","XYZ"});
        */
    }

    {
        // alternative over key-value pairs

        constexpr auto key1 = lit("key1") >> attr(key1_attr());
        constexpr auto key2 = lit("key2") >> attr(key2_attr());
        constexpr auto kv1 = key1 >> lit("=") >> +~char_(';');
        constexpr auto kv2 = key2 >> lit("=") >> +~char_(';');

        auto attr_ =  fusion::make_map<key1_attr, key2_attr>(std::string(),std::string());
        REQUIRE(parse("key2=XYZ;key1=ABC", (kv1|kv2) % ';', attr_));
        CHECK(fusion::at_key<key1_attr>(attr_) == "ABC");
        CHECK(fusion::at_key<key2_attr>(attr_) == "XYZ");
    }

    {
        // parsing sequence where key is a variant

        constexpr auto key1 = lit("key1") >> attr(key1_attr());
        constexpr auto key2 = lit("key2") >> attr(key2_attr());
        constexpr auto keys = key1 | key2;
        constexpr auto pair = keys >> lit("=") >> +~char_(';');

        {
            auto attr_ =  fusion::make_map<key1_attr,key2_attr>(std::string(),std::string());
            REQUIRE(parse("key1=ABC;key2=XYZ", pair % ';', attr_));
            CHECK(fusion::at_key<key1_attr>(attr_) == "ABC");
            CHECK(fusion::at_key<key2_attr>(attr_) == "XYZ");
        }
        {
            AdaptedStruct attr_;
            REQUIRE(parse("key1=ABC;key2=XYZ", pair % ';', attr_));
            CHECK(fusion::at_key<key1_attr>(attr_) == "ABC");
            CHECK(fusion::at_key<key2_attr>(attr_) == "XYZ");
        }
    }
}
