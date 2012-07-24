/*
  $Id: perftest.cc 5765 2010-10-19 04:36:20Z abehm $

  Copyright (C) 2010 by The Regents of the University of California
	
  Redistribution of this file is permitted under
  the terms of the BSD License.
    
  Date: 09/19/2007
  Author: Alexander Behm <abehm (at) ics.uci.edu>
*/

#include "statsgen.h"
#include "listmerger/src/mergeoptmerger.h"

void statsGenMemSimple();
void statsGenMemDiscardLists();
void statsGenMemCombineLists();
void statsGenDiskSimple();

void mem_dblp_author_mergeopt();
void mem_dblp_title_mergeopt();
void mem_imdb_actor_mergeopt();
void mem_google_mergeopt();
void mem_pubmed_mergeopt();

int main(int argc, char* argv[]) {
  
  //statsGenMemSimple();
  //statsGenMemDiscardLists();
  //statsGenMemCombineLists();
  //statsGenDiskSimple();

  // NOTE: the datasets used in these tests 
  // are not provided in the Flamingo Package due to their size
  // please contact Alexander Behm to obtain them
  //mem_dblp_author_mergeopt();
  //mem_dblp_title_mergeopt();
  //mem_imdb_actor_mergeopt();
  //mem_google_mergeopt();
  //mem_pubmed_mergeopt();
  
  return 0;
}

void statsGenMemSimple() {
  GramGenFixedLen gramGen(2);
  SimMetricEd simMetric(gramGen); // using the edit distance
  //SimMetricJacc simMetric(gramGen); // using jaccard similarity
  //SimMetricCos simMetric(gramGen); // using cosine similarity
  //SimMetricDice simMetric(gramGen); // using dice similarity
  float ed = 1.0f; // this represents the similarity threshold, in this case edit distance 1

  MergeOptMerger<> merger;
  typedef FtIndexerMem<> indexer;
  typedef FtSearcherMem<indexer> searcher;

  StatsGenConfig config;
  config.setGramGen(&gramGen);
  config.setSimMetric(&simMetric, ed); // similarity metric, similarity threshold
  config.setDictSize(4000, 4000, 4000); // size of string dictionary, start, stop, step
  config.setFanout(10); // fanout of filtertree
  config.setMaxStrLen(20);
  config.clearFilters();
  config.addPartFilter(new LengthFilter(20)); // add partitioning filter
  config.setNumberQueries(2000); // set total number of queries for the workload to run
  config.setDistinctQueries(2000); // set distinct number of queries generated by randomly picking strings from the dictionary
  config.setQueriesDistribution(QD_UNIFORM); // can be QD_UNIFORM or QD_ZIPF
  //config.setZipfSkew(1); // zipf skew parameter if QD_ZIPF specified
  config.setNumberRuns(3); // number times to repeat running workload to stabilize numbers
      
  StatsGen<indexer, searcher> statsgen(&config); // create an instance of the stats generator
  statsgen.setMerger(&merger);
  
  config.setDictDataFile("../data/female_names.txt"); // set the data file
  config.setOutputFlags(OF_WORKLOADSTATS | OF_QUERYRESULTS | OF_QUERYSTATS); // output workload stats, query stats and query results
  config.setOutputFilePrefix("memsimple_names_ed1"); // prefix of output files
  statsgen.generate(); // run the experiment

  cout << "SUCCESS!" << endl;
}

void statsGenMemDiscardLists() {
  GramGenFixedLen gramGen(3);
  SimMetricEd simMetric(gramGen); // using the edit distance
  float ed = 2.0f; // this represents the similarity threshold, in this case edit distance 1
  
  MergeOptMerger<> merger(true); // true == optimization for combining lists activated
  //typedef FtIndexerDiscardListsLLF<> indexer; // discard long lists first
  //typedef FtIndexerDiscardListsSLF<> indexer; // discard short lists first
  //typedef FtIndexerDiscardListsRandom<> indexer; // randomly discard lists first
  //typedef FtIndexerDiscardListsPanicCost<> indexer; // minimize number of panics
  typedef FtIndexerDiscardListsTimeCost<> indexer; // minimize total running time
  //typedef FtIndexerCombineListsBasic<> indexer; // combine lists based on correlation
  //typedef FtIndexerCombineListsCost<> indexer; // combine lists based on total running time
  typedef FtSearcherMem<indexer> searcher;
  
  StatsGenConfig config;
  config.setGramGen(&gramGen);
  config.setSimMetric(&simMetric, ed); // similarity metric, similarity threshold
  config.setSimMetric(&simMetric, ed); // similarity metric, similarity threshold
  config.setDictSize(4000, 4000, 4000); // size of string dictionary, start, stop, step
  config.setFanout(10); // fanout of filtertree
  config.setMaxStrLen(20);
  config.clearFilters();
  config.addPartFilter(new LengthFilter(20)); // add partitioning filter
  config.setNumberQueries(2000); // set total number of queries for the workload to run
  config.setDistinctQueries(2000); // set distinct number of queries generated by randomly picking strings from the dictionary
  config.setQueriesDistribution(QD_UNIFORM); // can be QD_UNIFORM or QD_ZIPF
  //config.setZipfSkew(1); // zipf skew parameter if QD_ZIPF specified
  config.setNumberRuns(3); // number times to repeat running workload to stabilize numbers
  
  // parameters for compression experiments
  config.setReduction(0.0, 0.51, 0.1); // reduction ratio start, stop, step
  config.setSampleRatioCost(false); // minimize cost or minimize benefit/cost? false means minimize cost (only for discarding lists)
  config.setQueriesSamplingFrac(0.25f); // use this sample of queries for compression
  config.setDictSamplingFrac(0.01f); // use this sample of data strings for compression

  StatsGen<indexer, searcher> statsgen(&config); // create an instance of the stats generator
  statsgen.setMerger(&merger);
  
  config.setDictDataFile("../data/female_names.txt"); // set the data file
  config.setOutputFlags(OF_WORKLOADSTATS | OF_QUERYRESULTS | OF_QUERYSTATS); // output workload stats, query stats and query results
  config.setOutputFilePrefix("memdl_names_ed1"); // prefix of output files
  statsgen.generate(); // run the experiment

  cout << "SUCCESS!" << endl;
}

void statsGenMemCombineLists() {
  GramGenFixedLen gramGen(3);
  SimMetricEd simMetric(gramGen); // using the edit distance
  float ed = 2.0f; // this represents the similarity threshold, in this case edit distance 1
  
  MergeOptMerger<> merger(true); // true == optimization for combining lists activated
  //typedef FtIndexerDiscardListsLLF<> indexer; // discard long lists first
  //typedef FtIndexerDiscardListsSLF<> indexer; // discard short lists first
  //typedef FtIndexerDiscardListsRandom<> indexer; // randomly discard lists first
  //typedef FtIndexerDiscardListsPanicCost<> indexer; // minimize number of panics
  //typedef FtIndexerDiscardListsTimeCost<> indexer; // minimize total running time
  //typedef FtIndexerCombineListsBasic<> indexer; // combine lists based on correlation
  typedef FtIndexerCombineListsCost<> indexer; // combine lists based on total running time
  typedef FtSearcherMem<indexer> searcher;
  
  StatsGenConfig config;
  config.setGramGen(&gramGen);
  config.setSimMetric(&simMetric, ed); // similarity metric, similarity threshold
  config.setDictSize(4000, 4000, 4000); // size of string dictionary, start, stop, step
  config.setFanout(10); // fanout of filtertree
  config.setMaxStrLen(20);
  config.clearFilters();
  config.addPartFilter(new LengthFilter(20)); // add partitioning filter
  config.setNumberQueries(2000); // set total number of queries for the workload to run
  config.setDistinctQueries(2000); // set distinct number of queries generated by randomly picking strings from the dictionary
  config.setQueriesDistribution(QD_UNIFORM); // can be QD_UNIFORM or QD_ZIPF
  //config.setZipfSkew(1); // zipf skew parameter if QD_ZIPF specified
  config.setNumberRuns(3); // number times to repeat running workload to stabilize numbers
  
  // parameters for compression experiments
  config.setReduction(0.0, 0.51, 0.1); // reduction ratio start, stop, step
  config.setSampleRatioCost(false); // minimize cost or minimize benefit/cost? false means minimize cost (only for discarding lists)
  config.setQueriesSamplingFrac(1.0f); // use this sample of queries for compression
  config.setDictSamplingFrac(0.01f); // use this sample of data strings for compression

  StatsGen<indexer, searcher> statsgen(&config); // create an instance of the stats generator
  statsgen.setMerger(&merger);
  
  config.setDictDataFile("../data/female_names.txt"); // set the data file
  config.setOutputFlags(OF_WORKLOADSTATS | OF_QUERYRESULTS | OF_QUERYSTATS); // output workload stats, query stats and query results
  config.setOutputFilePrefix("memcl_names_ed1"); // prefix of output files
  statsgen.generate(); // run the experiment

  cout << "SUCCESS!" << endl;
}

void statsGenDiskSimple() {
  GramGenFixedLen gramGen(2);
  SimMetricEd simMetric(gramGen); // using the edit distance
  //SimMetricJacc simMetric(gramGen); // using jaccard similarity
  //SimMetricCos simMetric(gramGen); // using cosine similarity
  //SimMetricDice simMetric(gramGen); // using dice similarity
  float ed = 1.0f; // this represents the similarity threshold, in this case edit distance 1

  OnDiskMergerSimple<> merger;
  typedef FtIndexerOnDisk<> indexer;
  typedef FtSearcherOnDisk<indexer> searcher;
  
  StatsGenConfig config;
  config.setGramGen(&gramGen);
  config.setSimMetric(&simMetric, ed); // similarity metric, similarity threshold
  config.setDictSize(4000, 4000, 4000); // size of string dictionary, start, stop, step
  config.setFanout(10); // fanout of filtertree
  config.setMaxStrLen(20);
  config.clearFilters();
  config.addPartFilter(new LengthFilter(20)); // add partitioning filter
  config.setNumberQueries(2000); // set total number of queries for the workload to run
  config.setDistinctQueries(2000); // set distinct number of queries generated by randomly picking strings from the dictionary
  config.setQueriesDistribution(QD_UNIFORM); // can be QD_UNIFORM or QD_ZIPF
  //config.setZipfSkew(1); // zipf skew parameter if QD_ZIPF specified
  config.setNumberRuns(3); // number times to repeat running workload to stabilize numbers
  
  config.setRunBuffer(50000); // size of buffer for index construction (in bytes)
  config.setAvgStrLen(8); // for optimizing disk-based storage of strings
  config.setPhysOrd(PHO_AUTO); // automatically choose physical ordering of strings in container  
  config.setPostMergeFilter(PMF_CSF_OPT); // use optimized charsum filter  

  // for experiments on raw disk IOs, clear file-system caches and disable c++ filestream buffering?
  // if set to true, perftest MUST be run as root user, sudo is NOT good enough
  // it is recommended to run perftest as root anyway, in order to clear caches after index construction, 
  // otherwise queries may seem too fast due to a hot filesystem cache
  config.setDropCachesBeforeBuildingIndex(false);
  config.setDropCachesBeforeEachQuery(false);
  config.setDisableStreamBuffer(false);
  
  StatsGen<indexer, searcher> statsgen(&config); // create an instance of the stats generator
  statsgen.setMerger(&merger);
  
  config.setDictDataFile("../data/female_names.txt"); // set the data file
  config.setOutputFlags(OF_WORKLOADSTATS | OF_QUERYRESULTS | OF_QUERYSTATS); // output workload stats, query stats and query results
  config.setOutputFilePrefix("disk_names_ed1"); // prefix of output files
  statsgen.generate(); // run the experiment
  
  cout << "SUCCESS!" << endl;
}

void mem_dblp_author_mergeopt() {
  GramGenFixedLen gramGen(3);
  SimMetricEd simMetric(gramGen);
  
  MergeOptMerger<> merger;
  typedef FtIndexerMem<> indexer;
  typedef FtSearcherMem<indexer, MergeOptMerger<> > searcher;

  StatsGenConfig config;

  StatsGen<indexer, searcher> statsgen(&config); // create an instance of the stats generator
  statsgen.setMerger(&merger);
  
  config.setGramGen(&gramGen);
  
  config.setDictDataFile("../data/bedcmp/data-dblp-author_flamingo.txt");
  config.setDictSize(2948929, 2948929, 2948929);
  
  config.setFanout(50);
  config.setMaxStrLen(50);
  config.setAutoPartFilter(false);
  config.addPartFilter(new LengthFilter(50));
  config.setNumberQueries(10000);
  config.setDistinctQueries(10000);
  config.setQueriesDistribution(QD_UNIFORM);
  config.setNumberRuns(3);      
  config.setOutputFlags(OF_WORKLOADSTATS | OF_QUERYRESULTS | OF_QUERYSTATS);
  config.setQueryType(QueryRange); // could also be QueryTopk
  
  //config.setPhysOrd(PHO_LENGTH); // sort data strings by length
  config.useExistingWorkload("../data/workload-dblp-author.txt");

  config.setSimMetric(&simMetric, 1.0f);
  config.setOutputFilePrefix("experiments/dblpauthor_mem_ed1");
  statsgen.generate();

  config.setSimMetric(&simMetric, 2.0f);
  config.setOutputFilePrefix("experiments/dblpauthor_mem_ed2");
  statsgen.generate();
  
  config.setSimMetric(&simMetric, 3.0f);
  config.setOutputFilePrefix("experiments/dblpauthor_mem_ed3");
  //statsgen.generate();
  
  config.setSimMetric(&simMetric, 4.0f);
  config.setOutputFilePrefix("experiments/dblpauthor_mem_ed4");
  //statsgen.generate();
}

void mem_dblp_title_mergeopt() {
  GramGenFixedLen gramGen(3);
  SimMetricEd simMetric(gramGen);
    
  MergeOptMerger<> merger;
  typedef FtIndexerMem<> indexer;
  typedef FtSearcherMem<indexer, MergeOptMerger<> > searcher;

  StatsGenConfig config;

  StatsGen<indexer, searcher> statsgen(&config); // create an instance of the stats generator
  statsgen.setMerger(&merger);
  
  config.setGramGen(&gramGen);
  
  config.setDictDataFile("../data/bedcmp/data-dblp-title_flamingo.txt");
  config.setDictSize(1158648, 1158648, 1158648);    
  
  config.setFanout(50);
  config.setMaxStrLen(800);
  config.setAutoPartFilter(false);
  config.addPartFilter(new LengthFilter(800));
  config.setNumberQueries(10000);
  config.setDistinctQueries(10000);
  config.setQueriesDistribution(QD_UNIFORM);
  config.setNumberRuns(3);    
  config.setOutputFlags(OF_WORKLOADSTATS | OF_QUERYRESULTS | OF_QUERYSTATS);
  config.setQueryType(QueryRange); // could also be QueryTopk    

  //config.setPhysOrd(PHO_LENGTH); // sort data strings by length
  config.useExistingWorkload("../data/workload-dblp-title.txt");

  config.setSimMetric(&simMetric, 1.0f);
  config.setOutputFilePrefix("experiments/dblptitle_mem_ed1");
  statsgen.generate();
  
  config.setSimMetric(&simMetric, 2.0f);
  config.setOutputFilePrefix("experiments/dblptitle_mem_ed2");
  statsgen.generate();

  config.setSimMetric(&simMetric, 3.0f);
  config.setOutputFilePrefix("experiments/dblptitle_mem_ed3");
  //statsgen.generate();
  
  config.setSimMetric(&simMetric, 4.0f);
  config.setOutputFilePrefix("experiments/dblptitle_mem_ed4");
  //statsgen.generate();
}

void mem_imdb_actor_mergeopt() {
  GramGenFixedLen gramGen(3);
  SimMetricEd simMetric(gramGen);
  
  MergeOptMerger<> merger;
  typedef FtIndexerMem<> indexer;
  typedef FtSearcherMem<indexer, MergeOptMerger<> > searcher;

  StatsGenConfig config;

  StatsGen<indexer, searcher> statsgen(&config); // create an instance of the stats generator
  statsgen.setMerger(&merger);
  
  config.setGramGen(&gramGen);
  
  config.setDictDataFile("../data/bedcmp/data-imdb-actor_flamingo.txt");
  config.setDictSize(1213391, 1213391, 1213391);
  
  config.setFanout(80);
  config.setMaxStrLen(80);
  config.clearFilters();
  config.setAutoPartFilter(false);
  config.addPartFilter(new LengthFilter(80));
  config.setNumberQueries(10000);
  config.setDistinctQueries(10000);
  config.setQueriesDistribution(QD_UNIFORM);
  config.setNumberRuns(3);     
  config.setOutputFlags(OF_WORKLOADSTATS | OF_QUERYRESULTS | OF_QUERYSTATS);
  config.setQueryType(QueryRange); // could also be QueryTopk

  //config.setPhysOrd(PHO_LENGTH); // sort data strings by length
  config.useExistingWorkload("../data/workload-imdb-actor.txt");

  config.setSimMetric(&simMetric, 1.0f);
  config.setOutputFilePrefix("experiments/imdbactor_mem_ed1");
  statsgen.generate();

  config.setSimMetric(&simMetric, 2.0f);
  config.setOutputFilePrefix("experiments/imdbactor_mem_ed2");
  statsgen.generate();

  config.setSimMetric(&simMetric, 3.0f);
  config.setOutputFilePrefix("experiments/imdbactor_mem_ed3");
  //statsgen.generate();

  config.setSimMetric(&simMetric, 4.0f);
  config.setOutputFilePrefix("experiments/imdbactor_mem_ed4");
  //statsgen.generate();
}


void mem_google_mergeopt() {
  GramGenFixedLen gramGen(3);
  SimMetricEd simMetric(gramGen);
  
  MergeOptMerger<> merger;
  typedef FtIndexerMem<> indexer;
  typedef FtSearcherMem<indexer, MergeOptMerger<> > searcher;
  
  StatsGenConfig config;

  StatsGen<indexer, searcher> statsgen(&config); // create an instance of the stats generator
  statsgen.setMerger(&merger);
  
  config.setGramGen(&gramGen);
  
  config.setDictDataFile("../data/googlebig250.txt");
  config.setDictSize(2000000, 2000000, 2000000);    
  
  config.setFanout(100);
  config.setMaxStrLen(200);
  config.clearFilters();
  config.setAutoPartFilter(false);
  config.addPartFilter(new LengthFilter(200));
  config.setNumberQueries(10000);
  config.setDistinctQueries(10000);
  config.setQueriesDistribution(QD_UNIFORM);
  config.setNumberRuns(3);    
  config.setOutputFlags(OF_WORKLOADSTATS | OF_QUERYRESULTS | OF_QUERYSTATS);
  config.setQueryType(QueryRange); // could also be QueryTopk  

  //config.setPhysOrd(PHO_LENGTH); // sort data strings by length
  config.useExistingWorkload("../data/workload-google.txt");
  
  config.setSimMetric(&simMetric, 1.0f);
  config.setOutputFilePrefix("experiments/google_mem_ed1");
  statsgen.generate();

  config.setSimMetric(&simMetric, 2.0f);
  config.setOutputFilePrefix("experiments/google_mem_ed2");
  statsgen.generate();

  config.setSimMetric(&simMetric, 3.0f);
  config.setOutputFilePrefix("experiments/google_mem_ed3");
  //statsgen.generate();

  config.setSimMetric(&simMetric, 4.0f);
  config.setOutputFilePrefix("experiments/google_mem_ed4");
  //statsgen.generate();
}

void mem_pubmed_mergeopt() {
  GramGenFixedLen gramGen(3);
  SimMetricEd simMetric(gramGen);
  
  MergeOptMerger<> merger;
  typedef FtIndexerMem<> indexer;
  typedef FtSearcherMem<indexer, MergeOptMerger<> > searcher;
  
  StatsGenConfig config;

  StatsGen<indexer, searcher> statsgen(&config); // create an instance of the stats generator
  statsgen.setMerger(&merger);
  
  config.setGramGen(&gramGen);
  
  config.setDictDataFile("../data/pubmedtitles250.txt");
  config.setDictSize(2000000, 2000000, 2000000);    
  
  config.setFanout(100);
  config.setMaxStrLen(300);
  config.clearFilters();
  config.setAutoPartFilter(false);
  config.addPartFilter(new LengthFilter(300));
  config.setNumberQueries(10000);
  config.setDistinctQueries(10000);
  config.setQueriesDistribution(QD_UNIFORM);
  config.setNumberRuns(3);
  config.setOutputFlags(OF_WORKLOADSTATS | OF_QUERYRESULTS | OF_QUERYSTATS);
  config.setQueryType(QueryRange); // could also be QueryTopk    
  
  //config.setPhysOrd(PHO_LENGTH); // sort data strings by length
  config.useExistingWorkload("../data/workload-pubmed.txt");

  config.setSimMetric(&simMetric, 1.0f);
  config.setOutputFilePrefix("experiments/pubmed_mem_ed1");
  statsgen.generate();

  config.setSimMetric(&simMetric, 2.0f);
  config.setOutputFilePrefix("experiments/pubmed_mem_ed2");
  statsgen.generate();
  
  config.setSimMetric(&simMetric, 3.0f);
  config.setOutputFilePrefix("experiments/pubmed_mem_ed3");
  //statsgen.generate();

  config.setSimMetric(&simMetric, 4.0f);
  config.setOutputFilePrefix("experiments/pubmed_mem_ed4");
  //statsgen.generate();
}