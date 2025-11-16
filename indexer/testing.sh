#!/bin/bash
# testing.sh 
# CS50 FA25 Final Project 'indexer' Module
# Author: Cindy Jiayi Liu, Group 1
# Date: 2025.11.15

# Ensure the directory for test data doesn't exist, then create it
rm -rf testing
mkdir testing

################## indexer #######################

################## Test 1: error cases, corner cases #######################
# no arguments
./indexer

# only 1 argument
./indexer ../crawler/testing/letters-1

# three or more arguments
./indexer 1 2 3 4 5

################## Test 2: valgrind test #######################
# valgrind test for letters-0
valgrind ./indexer ~/cs50-dev/shared/tse/output/letters-0 testing/letters-0.index

# valgrind test for letters-1
valgrind ./indexer ~/cs50-dev/shared/tse/output/letters-1 testing/letters-1.index

# valgrind test for letters-2
valgrind ./indexer ~/cs50-dev/shared/tse/output/letters-2 testing/letters-2.index

# valgrind test for toscrape-1
valgrind ./indexer ~/cs50-dev/shared/tse/output/toscrape-1 testing/toscrape-1.index

# valgrind test for wikipedia-1
valgrind ./indexer ~/cs50-dev/shared/tse/output/wikipedia-1 testing/wikipedia-1.index

################## Test 3: check output file #######################
# indexcmp for letter-0.index
./indexer ~/cs50-dev/shared/tse/output/letters-0 testing/letters-0.index
~/cs50-dev/shared/tse/indexcmp testing/letters-0.index ~/cs50-dev/shared/tse/output/letters-0.index

# indexcmp for letter-1.index
./indexer ~/cs50-dev/shared/tse/output/letters-1 testing/letters-1.index
~/cs50-dev/shared/tse/indexcmp testing/letters-1.index ~/cs50-dev/shared/tse/output/letters-1.index

# indexcmp for letter-2.index
./indexer ~/cs50-dev/shared/tse/output/letters-2 testing/letters-2.index
~/cs50-dev/shared/tse/indexcmp testing/letters-2.index ~/cs50-dev/shared/tse/output/letters-2.index

# indexcmp for toscrape-1.index
./indexer ~/cs50-dev/shared/tse/output/toscrape-1 testing/toscrape-1.index
~/cs50-dev/shared/tse/indexcmp testing/toscrape-1.index ~/cs50-dev/shared/tse/output/toscrape-1.index

# indexcmp for wikipedia-1.index
./indexer ~/cs50-dev/shared/tse/output/wikipedia-1 testing/wikipedia-1.index
~/cs50-dev/shared/tse/indexcmp testing/wikipedia-1.index ~/cs50-dev/shared/tse/output/wikipedia-1.index

################## indextest #######################

################## Test 1: error cases, corner cases #######################
# no arguments
./indextest

# only 1 argument
./indextest testing/letters-1

# three or more arguments
./indextest 1 2 3 4 5

# invalid oldIndexFilePath
./indextest testing/nonexist/index.index testing/tmp.index

################## Test 2: valgrind test #######################
# valgrind test for letters-0
valgrind  ./indextest testing/letters-0.index testing/letters-0-test.index

# valgrind test for letters-1
valgrind  ./indextest testing/letters-1.index testing/letters-1-test.index

# valgrind test for letters-2
valgrind  ./indextest testing/letters-2.index testing/letters-2-test.index

# valgrind test for toscrape-1
valgrind  ./indextest testing/toscrape-1.index testing/toscrape-1-test.index

# valgrind test for wikipedia-1
valgrind  ./indextest testing/wikipedia-1.index testing/wikipedia-1-test.index

################## Test 3: function test #######################
# indextest for letters-0
./indextest testing/letters-0.index testing/letters-0-test.index
~/cs50-dev/shared/tse/indexcmp testing/letters-0.index testing/letters-0-test.index

# indextest for letters-1
./indextest testing/letters-1.index testing/letters-1-test.index
~/cs50-dev/shared/tse/indexcmp testing/letters-1.index testing/letters-1-test.index

# indextest for letters-2
./indextest testing/letters-2.index testing/letters-2-test.index
~/cs50-dev/shared/tse/indexcmp testing/letters-2.index testing/letters-2-test.index

# indextest for toscrape-1
./indextest testing/toscrape-1.index testing/toscrape-1-test.index
~/cs50-dev/shared/tse/indexcmp testing/toscrape-1.index testing/toscrape-1-test.index

# indextest for wikipedia-1
./indextest testing/wikipedia-1.index testing/wikipedia-1-test.index
~/cs50-dev/shared/tse/indexcmp testing/wikipedia-1.index testing/wikipedia-1-test.index
