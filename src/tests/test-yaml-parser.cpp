#include <chrono>
#include <cmath>
#include <iostream>

#include <parsers/yaml/yaml.hpp>

#include "pch.h"
namespace chrono = std::chrono;
using namespace yaml;

TEST(TestYamlParser, BasicTest) {
  using namespace std::chrono;
  Entry entry(Entry::Type::kMap, nullptr);

  std::vector<int> t{1024, 768, 123};
  entry["Resolution"]["Seq"] = t;
  entry["Resolution"]["Map"] =
      std::map<std::string, int>{{"x", 1024}, {"y", 768}};
  std::map<std::string, int> map{
      {"first", 1}, {"second", 2}, {"third", 3}, {"fourth", 4}};

  entry["data"]["thing"] = std::move(map);

  auto now = system_clock::now();

  auto const ld = floor<days>(now);
  const year_month_day ymd{ld};
  hh_mm_ss hms{duration_cast<microseconds>(now - ld)};
  std::tm tm = {0};
  auto cast = duration_cast<seconds>(now.time_since_epoch());
  tm.tm_sec = floor<seconds>(cast).count() % 60;
  tm.tm_min = floor<minutes>(cast).count() % 60;
  tm.tm_hour = floor<hours>(cast).count() % 24;
  year_month_day ymd2 = floor<days>(sys_seconds(cast));
  tm.tm_year = (int)ymd2.year();
  tm.tm_mon = (unsigned)ymd2.month();
  tm.tm_mday = (unsigned)ymd2.day();
  tm.tm_isdst = -1;
  entry["data"]["timestamp"] = tm;
  entry["data"]["date"] = ymd;
  entry["data"]["time"] = hms;
  entry["data"]["thing"]["first"] = entry["data"]["timestamp"];
  std::string str;
  for (std::string const &i : entry.Serialize()) {
    str.insert(str.size(), i);
    str += '\n';
  }
  for (Entry const &x : entry) {
    for (Entry const &y : x) {
      ASSERT_NO_THROW(ASSERT_NE(x, y));
    }
  }
}
TEST(TestYamlParser, TestCollections_SequenceOfScalars) {
  Entry entry = Parse(R"(
- Mark McGwire
- Sammy Sosa
- Ken Griffey)");
  ASSERT_EQ(entry.size(), 3);
  ASSERT_EQ(entry.type(), Entry::Type::kSequence);
  ASSERT_EQ(entry[0].str(), "Mark McGwire");
  ASSERT_EQ(entry[1].str(), "Sammy Sosa");
  ASSERT_EQ(entry[2].str(), "Ken Griffey");
  ASSERT_TRUE(entry.contains("Mark McGwire"));
  ASSERT_TRUE(entry.contains("Sammy Sosa"));
  ASSERT_TRUE(entry.contains("Ken Griffey"));
  for (Entry const &sub_entry : entry) {
    ASSERT_EQ(sub_entry.type(), Entry::Type::kString);
  }
}
TEST(TestYamlParser, TestCollections_MappingOfScalarsToScalars) {
  Entry entry = Parse(R"(
hr:  65    # Home runs
avg: 0.278 # Batting average
rbi: 147   # Runs Batted In)");
  ASSERT_EQ(entry.size(), 3);
  ASSERT_TRUE(entry.is_map());
  ASSERT_TRUE(entry["hr"].is_int());
  ASSERT_TRUE(entry["avg"].is_double());
  ASSERT_TRUE(entry["rbi"].is_int());
  ASSERT_EQ(entry["hr"].to_int(), 65);
  ASSERT_EQ(entry["avg"].to_double(), 0.278);
  ASSERT_EQ(entry["rbi"], 147); // operator== implicitly calls to_int()
}

TEST(TestYamlParser, TestCollections_MappingScalarsToSequences) {
  Entry entry = Parse(R"(
american:
- Boston Red Sox
- Detroit Tigers
- New York Yankees
national:
- New York Mets
- Chicago Cubs
- Atlanta Braves)");
  ASSERT_EQ(entry.size(), 2);
  Entry const &sub_entry1 = entry["american"];
  ASSERT_EQ(sub_entry1.size(), 3);
  ASSERT_TRUE(sub_entry1.contains("Boston Red Sox"));
  ASSERT_TRUE(sub_entry1.contains("Detroit Tigers"));
  ASSERT_TRUE(sub_entry1.contains("New York Yankees"));
  Entry const &sub_entry2 = entry["national"];
  ASSERT_EQ(sub_entry2.size(), 3);
  ASSERT_TRUE(sub_entry2.contains("New York Mets"));
  ASSERT_TRUE(sub_entry2.contains("Chicago Cubs"));
  ASSERT_TRUE(sub_entry2.contains("Atlanta Braves"));
}

TEST(TestYamlParser, TestCollections_SequenceOfMappings) {
  Entry entry = Parse(R"(
-
  name: Mark McGwire
  hr:   65
  avg:  0.278
-
  name: Sammy Sosa
  hr:   63
  avg:  0.288)");
  ASSERT_EQ(entry.size(), 2);
  ASSERT_TRUE(entry[0].is_map());
  ASSERT_EQ(entry[0]["name"].str(), "Mark McGwire");
  ASSERT_EQ(entry[0]["hr"].to_int(), 65);
  ASSERT_EQ(entry[0]["avg"].to_double(), 0.278);
  ASSERT_TRUE(entry[1].is_map());
  ASSERT_EQ(entry[1]["name"].str(), "Sammy Sosa");
  ASSERT_EQ(entry[1]["hr"].to_int(), 63);
  ASSERT_EQ(entry[1]["avg"].to_double(), 0.288);
}

TEST(TestYamlParser, TestCollections_SequenceOfSequences) {
  Entry entry = Parse(R"(
- [name        , hr, avg  ]
- [Mark McGwire, 65, 0.278]
- [Sammy Sosa  , 63, 0.288])");
  ASSERT_EQ(entry.size(), 3);
  ASSERT_TRUE(entry.is_sequence());
  ASSERT_TRUE(entry[0].is_sequence());
  ASSERT_TRUE(entry[0].contains("name") && entry[0].contains("hr") &&
              entry[0].contains("avg"));
  ASSERT_TRUE(entry[1].is_sequence());
  ASSERT_TRUE(entry[1].contains("Mark McGwire") && entry[1].contains(65) &&
              entry[1].contains(0.278));
  ASSERT_TRUE(entry[2].is_sequence());
  ASSERT_TRUE(entry[2].contains("Sammy Sosa") && entry[2].contains(63) &&
              entry[2].contains(0.288));
}
TEST(TestYamlParser, TestCollections_MappingOfMappings) {
  Entry entry = Parse(R"(
Mark McGwire: {hr: 65, avg: 0.278}
Sammy Sosa: {
  hr:
    63, avg : 0.288,
 })");
  ASSERT_EQ(entry.size(), 2);
  ASSERT_TRUE(entry.is_map());
  ASSERT_TRUE(entry["Mark McGwire"].is_map());
  ASSERT_EQ(entry["Mark McGwire"]["hr"].to_int(), 65);
  ASSERT_EQ(entry["Mark McGwire"]["avg"].to_double(), 0.278);
  ASSERT_TRUE(entry["Sammy Sosa"].is_map());
  ASSERT_EQ(entry["Sammy Sosa"]["hr"].to_int(), 63);
  ASSERT_EQ(entry["Sammy Sosa"]["avg"].to_double(), 0.288);
}

TEST(TestYamlParser, TestStructures_TwoDocuments) {
  Entry entry = Parse(R"(
# Ranking of 1998 home runs
---
- Mark McGwire
- Sammy Sosa
- Ken Griffey

#Team ranking
---
- Chicago Cubs
- St Louis Cardinals)");
  ASSERT_TRUE(entry.is_sequence());
  ASSERT_TRUE(entry[0].is_directive());
  ASSERT_EQ(entry[0][0].str(), "Mark McGwire");
  ASSERT_EQ(entry[0][1].str(), "Sammy Sosa");
  ASSERT_EQ(entry[0][2].str(), "Ken Griffey");
  ASSERT_TRUE(entry[1].is_directive());
  ASSERT_EQ(entry[1][0].str(), "Chicago Cubs");
  ASSERT_EQ(entry[1][1].str(), "St Louis Cardinals");
}

TEST(TestYamlParser, TestStructures_TwoDocumentsInOneStream) {
  Entry entry = Parse(R"(
---
time: 20:03:20
player: Sammy Sosa
action: strike (miss)
...
---
time: 20:03:47
player: Sammy Sosa
action: grand slam
...)");
  ASSERT_TRUE(entry.is_sequence());
  ASSERT_TRUE(entry[0].is_directive());
  ASSERT_EQ(entry[0][0]["time"].type(), Entry::Type::kTime);
  ASSERT_EQ(entry[0][0]["time"].str(), "20:03:47");
  ASSERT_EQ(entry[0][0]["player"].str(), "Sammy Sosa");
  ASSERT_EQ(entry[0][0]["action"].str(), "strike (miss)");
  ASSERT_TRUE(entry[1].is_directive());
  ASSERT_EQ(entry[1][0]["time"].type(), Entry::Type::kTime);
  ASSERT_EQ(entry[1][0]["time"].str(), "20:03:47");
  ASSERT_EQ(entry[1][0]["player"].str(), "Sammy Sosa");
  ASSERT_EQ(entry[1][0]["action"].str(), "grand slam");
}

TEST(TestYamlParser, TestStructures_SingleDocumentWithTwoComments) {
  Entry entry = Parse(R"(
---
hr: # 1998 hr ranking
- Mark McGwire
- Sammy Sosa
# 1998 rbi ranking
rbi:
- Sammy Sosa
- Ken Griffey)");
  ASSERT_TRUE(entry.is_map());
  ASSERT_TRUE(entry["hr"].is_sequence());
  ASSERT_TRUE(entry["hr"].contains("Mark McGwire"));
  ASSERT_TRUE(entry["hr"].contains("Sammy Sosa"));
  ASSERT_TRUE(entry["rbi"].is_sequence());
  ASSERT_TRUE(entry["rbi"].contains("Sammy Sosa"));
  ASSERT_TRUE(entry["rbi"].contains("Ken Griffey"));
}
TEST(TestYamlParser, TestStructures_Nodes) {
  Entry entry = Parse(R"(
---
hr:
- Mark McGwire
#Following node labeled SS
- &SS Sammy Sosa
rbi:
- *SS # Subsequent occurrence
- Ken Griffey)");
  ASSERT_TRUE(entry.is_map());
  ASSERT_TRUE(entry["hr"].is_sequence());
  ASSERT_TRUE(entry["hr"].contains("Mark McGwire"));
  ASSERT_TRUE(entry["hr"].contains("Sammy Sosa"));
  ASSERT_TRUE(entry["rbi"].is_sequence());
  ASSERT_TRUE(entry["rbi"].contains("Sammy Sosa"));
  ASSERT_TRUE(entry["rbi"].contains("Ken Griffey"));
}
TEST(TestYamlParser, TestStructures_MappingBetweenSequences) {
  Entry entry = Parse(R"(
? - Detroit Tigers
  - Chicago cubs
: - 2001-07-23

? [ New York Yankees,
    Atlanta Braves ]
: [ 2001-07-02, 2001-08-12,
    2001-08-14 ])");
  ASSERT_TRUE(entry.is_map());
  ASSERT_TRUE(entry[0].is_pair() && entry[0].key().is_sequence() &&
              entry[0].value().is_date());
  ASSERT_EQ(entry[0].key()[0].str(), "Detroit Tigers");
  ASSERT_EQ(entry[0].key()[1].str(), "Chicago cubs");
  ASSERT_EQ(entry[0].value().str(), "2001-07-23");
  ASSERT_TRUE(entry[1].is_pair() && entry[1].key().is_sequence() &&
              entry[1].value().is_sequence());
  ASSERT_EQ(entry[1].key()[0], "New York Yankees");
  ASSERT_EQ(entry[1].key()[1], "Atlanta Braves");
  ASSERT_EQ(entry[1].value()[0], "2001-07-02");
  ASSERT_EQ(entry[1].value()[1], "2001-08-12");
  ASSERT_EQ(entry[1].value()[2], "2001-08-14");
}
TEST(TestYamlParser, TestStructures_CompactNestedMapping) {
  Entry entry = Parse(R"(
---
#Products purchased
- item    : Super Hoop
  quantity: 1
- item    : Basketball
  quantity: 4
- item    : Big Shoes
  quantity: 1)");
  ASSERT_TRUE(entry.is_sequence());
  ASSERT_EQ(entry[0]["item"], "Super Hoop");
  ASSERT_EQ(entry[0]["quantity"], 1);
  ASSERT_EQ(entry[1]["item"], "Basketball");
  ASSERT_EQ(entry[1]["quantity"], 4);
  ASSERT_EQ(entry[2]["item"], "Big Shoes");
  ASSERT_EQ(entry[2]["quantity"], 1);
}
TEST(TestYamlParser, TestScalars_PreservedNewlines) {
  Entry entry = Parse(R"(
# ASCII Art
--- |
  \//||\/||
  // ||  ||__)");
  ASSERT_TRUE(entry.is_string());
  ASSERT_EQ(entry, "\\//||\\/||\n// ||  ||__");
}
TEST(TestYamlParser, TestScalars_FoldedNewlines) {
  Entry entry = Parse(R"(
--- >
  Mark McGwire's
  year was crippled
  by a knee injury.)");
  ASSERT_TRUE(entry.is_string());
  ASSERT_EQ(entry, "Mark McGwire's year was crippled by a knee injury.");
}
TEST(TestYamlParser, TestScalars_FoldedNewlinesWithIndents) {
  Entry entry = Parse(R"(
--- >
 Sammy Sosa completed another
 fine season with great stats.

   63 Home Runs
   0.288 Batting Average

 What a year!)");
  // TODO: ASSERTS FOR THIS ENTRY
}
TEST(TestYamlParser, TestScalars_FoldedNewlineScopes) {
  Entry entry = Parse(R"(
name: Mark McGwire
accomplishment: >
  Mark set a major league
  home run record in 1998.
stats: |
  65 Home Runs
  0.278 Batting Average)");
  ASSERT_TRUE(entry.is_map());
  ASSERT_EQ(entry["name"], "Mark McGwire");
  ASSERT_TRUE(entry["accomplishment"].is_string());
  ASSERT_EQ(entry["accomplishment"],
            "Mark set a major league home run record in 1998.");
  ASSERT_TRUE(entry["stats"].is_string());
  ASSERT_EQ(entry["stats"], "65 Home Runs\n0.278 Batting Average");
}
TEST(TestYamlParser, TestScalars_QuotedScalars) {
  Entry entry = Parse(R"(
unicode: "Sosa did fine.\u263A"
control: "\b1998\t1999\t2000\n"
hex esc: "\x0d\x0a is \r\n"

single: '"Howdy!" he cried.'
quoted: ' # Not a ''comment''.'
tie-fighter: '|\-*-/|')");
  ASSERT_EQ(entry["unicode"], "Sosa did fine.\x26\x3A");
  ASSERT_EQ(entry["control"], "\b1998\t1999\t2000\n");
  ASSERT_EQ(entry["hex esc"], "\x0d\x0a is \r\n");
  ASSERT_EQ(entry["hex esc"], "\"Howdy!\" he cried.");
  ASSERT_EQ(entry["hex esc"], " # Not a ' 'comment' '.");
  ASSERT_EQ(entry["hex esc"], "|\\-*-/|");
}
TEST(TestYamlParser, TestScalars_MultiLineFlowScalars) {
  Entry entry = Parse(R"(
plain:
  This unquoted scalar
  spans many lines.

quoted: "So does this
  quoted scalar.\n")");
  ASSERT_TRUE(entry.is_map());
  ASSERT_TRUE(entry["plain"].is_string());
  ASSERT_EQ(entry["plain"], "This unquoted scalar spans many lines.\n");
  ASSERT_TRUE(entry["quoted"].is_string());
  ASSERT_EQ(entry["quoted"], "So does this quoted scalar.\n");
}
TEST(TestYamlParser, TestTags_Integers) {
  Entry entry = Parse(R"(
canonical: 12345
decimal: +12345
octal: 0o14
hexadecimal: 0xC)");
  ASSERT_EQ(entry["canonical"], 12345);
  ASSERT_EQ(entry["decimal"], 12345);
  ASSERT_EQ(entry["octal"], 12);
  ASSERT_EQ(entry["hexadecimal"], 12);
}
TEST(TestYamlParser, TestTags_FloatingPoint) {
  Entry entry = Parse(R"(
canonical: 1.23015e+3
exponential: 12.3015e+02
fixed: 1230.15
negative infinity: -.inf
not a number: .nan)");
  ASSERT_EQ(entry["canonical"], 1230.15);
  ASSERT_EQ(entry["exponential"], 1230.15);
  ASSERT_EQ(entry["fixed"], 1230.15);
  ASSERT_EQ(entry["negative infinity"],
            -std::numeric_limits<long double>::infinity());
  ASSERT_EQ(entry["positive infinity"],
            std::numeric_limits<long double>::infinity());
  ASSERT_TRUE(std::isnan(entry["not a number"].to_double()));
}
TEST(TestYamlParser, TestTags_Miscellaneous) {
  Entry entry = Parse(R"(
null:
booleans: [ true, false ]
string: '012345')");
  ASSERT_TRUE(entry["null"].is_null());
  ASSERT_TRUE(entry["booleans"].is_sequence());
  ASSERT_EQ(entry["booleans"][0], true);
  ASSERT_EQ(entry["booleans"][1], false);
  ASSERT_EQ(entry["string"], "012345");
}
TEST(TestYamlParser, TestTags_Timestamps) {
  Entry entry = Parse(R"(
canonical: 2001-12-15T02:59:43.1Z
iso8601: 2001-12-14t21:59:43.10-05:00
spaced: 2001-12-14 21:59:43.10 -5
date: 2002-12-14)");
  ASSERT_TRUE(entry["canonical"].is_timestamp());
  ASSERT_TRUE(entry["iso8601"].is_timestamp());
  ASSERT_TRUE(entry["spaced"].is_timestamp());
  ASSERT_TRUE(entry["date"].is_date());
  // TODO: add asserts that check if the time is correct
}
TEST(TestYamlParser, TestTags_VariousExplicitTags) {
  Entry entry = Parse(R"(
---
not-date: !!str 2002-04-28

picture: !!binary |
 R0lGODlhDAAMAIQAAP//9/X
 17unp5WZmZgAAAOfn515eXv
 Pz7Y6OjuDg4J+fn5OTk6enp
 56enmleECcgggoBADs=

application specific tag: !something |
 The semantics of the tag
 above may be different for
 different documents.)");
  ASSERT_TRUE(entry["not-date"].is_string() && !entry["not-date"].is_date());
  // TODO: add assert to test if the binary string is correct
}
TEST(TestYamlParser, TestTags_GlobalTags) {
  Entry entry = Parse(R"(
%TAG ! tag:clarkevans.com,2002:
--- !shape
#Use the !handle for presenting
#tag : clarkevans.com, 2002 : circle
- !circle
  center: &ORIGIN {x: 73, y: 129}
  radius: 7
- !line
  start: *ORIGIN
  finish: {
  x:
    89, y : 102 }
- !label
  start: *ORIGIN
  color: 0xFFEEBB
  text: Pretty vector drawing.)");
  ASSERT_EQ(entry.tag(), "shape");
  ASSERT_TRUE(entry.is_sequence());
  ASSERT_EQ(entry[0].tag(), "circle");
  ASSERT_EQ(entry[1].tag(), "line");
  ASSERT_EQ(entry[2].tag(), "label");
}
TEST(TestYamlParser, TestTags_UnorderedSets) {
  Entry entry = Parse(R"(
# Sets are represented as a
#Mapping where each key is
#associated with a null value
--- !!set
? Mark McGwire
? Sammy Sosa
? Ken Griffey)");
  ASSERT_EQ(entry.tag(), "set");
  ASSERT_TRUE(entry.is_set());
  ASSERT_TRUE(entry["Mark McGwire"].is_null());
  ASSERT_TRUE(entry["Sammy Sosa"].is_null());
  ASSERT_TRUE(entry["Ken Griffey"].is_null());
}
TEST(TestYamlParser, TestTags_OrderedMappings) {
  Entry entry = Parse(R"(
# Ordered maps are represented as
#A sequence of mappings, with
#each mapping having one key
--- !!omap
- Mark McGwire: 65
- Sammy Sosa: 63
- Ken Griffey: 58)");
  ASSERT_EQ(entry.tag(), "omap");
  ASSERT_TRUE(entry.is_map());
  ASSERT_EQ(entry["Mark McGwire"], 65);
  ASSERT_EQ(entry["Sammy Sosa"], 63);
  ASSERT_EQ(entry["Ken Griffey"], 58);
}

TEST(TestYamlParser, FullTest_InvoiceTest) {
  Entry entry = Parse(R"(
--- !<tag:clarkevans.com,2002:invoice>
invoice: 34843
date   : 2001-01-23
bill-to: &id001
  given  : Chris
  family : Dumars
  address:
    lines: |
      458 Walkman Dr.
      Suite #292
    city    : Royal Oak
    state   : MI
    postal  : 48046
ship-to: *id001
product:
- sku         : BL394D
  quantity    : 4
  description : Basketball
  price       : 450.00
- sku         : BL4438H
  quantity    : 1
  description : Super Hoop
  price       : 2392.00
tax  : 251.42
total: 4443.52
comments:
  Late afternoon is best.
  Backup contact is Nancy
  Billsmer @ 338-4338.)");
  using namespace std::chrono;
  ASSERT_EQ(entry["invoice"], 34843);
  ASSERT_EQ(entry["date"], "2001-01-23");
  ASSERT_EQ(entry["date"], year_month_day(year(2001), month(1), day(23)));
  ASSERT_EQ(entry["bill-to"]["given"], "Chris");
  ASSERT_EQ(entry["bill-to"]["family"], "Dumars");
  ASSERT_EQ(entry["bill-to"]["address"]["lines"], "458 Walkman Dr. Suite #292");
  ASSERT_EQ(entry["bill-to"]["address"]["city"], "Royal Oak");
  ASSERT_EQ(entry["bill-to"]["address"]["state"], "MI");
  ASSERT_EQ(entry["bill-to"]["address"]["postal"], 48046);
  ASSERT_EQ(entry["ship-to"], entry["bill-to"]);
  ASSERT_EQ(entry["ship-to"]["given"], "Chris");
  ASSERT_EQ(entry["ship-to"]["family"], "Dumars");
  ASSERT_EQ(entry["ship-to"]["address"]["lines"], "458 Walkman Dr. Suite #292");
  ASSERT_EQ(entry["ship-to"]["address"]["city"], "Royal Oak");
  ASSERT_EQ(entry["ship-to"]["address"]["state"], "MI");
  ASSERT_EQ(entry["ship-to"]["address"]["postal"], 48046);
  ASSERT_EQ(entry["product"][0]["sku"], "BL349D");
  ASSERT_EQ(entry["product"][0]["quantity"], 4);
  ASSERT_EQ(entry["product"][0]["description"], "Basketball");
  ASSERT_EQ(entry["product"][0]["price"], 450.0);
  ASSERT_EQ(entry["product"][1]["sku"], "BL4438H");
  ASSERT_EQ(entry["product"][1]["quantity"], 1);
  ASSERT_EQ(entry["product"][1]["description"], "Super Hoop");
  ASSERT_EQ(entry["product"][1]["price"], 2392.0);
  ASSERT_EQ(entry["tax"], 251.42);
  ASSERT_EQ(entry["total"], 4443.52);
  ASSERT_EQ(
      entry["comments"],
      "Late afternoon is best. Backup contact is Nancy Billsmer @ 338-4338.");
}

TEST(TestYamlParser, FullTest_LogFile) {
  Entry entry = Parse(R"(---
Time: 2001-11-23 15:01:42 -5
User: ed
Warning:
  This is an error message
  for the log file
---
Time: 2001-11-23 15:02:31 -5
User: ed
Warning:
  A slightly different error
  message.
---
Date: 2001-11-23 15:03:17 -5
User: ed
Fatal:
  Unknown variable "bar"
Stack:
- file: TopClass.py
  line: 23
  code: |
    x = MoreObject("345\n")
- file: MoreClass.py
  line: 58
  code: |-
    foo = bar )");
  ASSERT_EQ(entry[0]["Time"], "2001-11-23 15:01:42 -5");
  ASSERT_EQ(entry[0]["User"], "ed");
  ASSERT_EQ(entry[0]["Warning"], "This is an error message for the log file");
  ASSERT_EQ(entry[1]["Time"], "2001-11-23 15:02:31 -5");
  ASSERT_EQ(entry[1]["User"], "ed");
  ASSERT_EQ(entry[1]["Warning"], "A slightly different error message.");
  ASSERT_EQ(entry[2]["Time"], "2001-11-23 15:03:17 -5");
  ASSERT_EQ(entry[2]["User"], "ed");
  ASSERT_EQ(entry[2]["Fatal"], "Unknown variable \"bar\"");
  ASSERT_EQ(entry[2]["Stack"][0]["file"], "TopClass.py");
  ASSERT_EQ(entry[2]["Stack"][0]["line"], 23);
  ASSERT_EQ(entry[2]["Stack"][0]["code"], "x  = MoreObject(\"345\n\")\n");
  ASSERT_EQ(entry[2]["Stack"][1]["file"], "MoreClass.py");
  ASSERT_EQ(entry[2]["Stack"][1]["line"], 58);
  ASSERT_EQ(entry[2]["Stack"][1]["code"], "foo = bar )\n");
}