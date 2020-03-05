#include <iostream>
#include <climits>

#include "../adventure.h"
#include "../utils.h"
#include "../../third_party/threadpool/threadpool.h"

#define TRIES 5
std::vector<Egg> test6_eggs;
uint64_t ans;
uint64_t W=(10000);
BottomlessBag test6_bag(W);

void correctnessTest(std::vector<Egg> eggs, BottomlessBag bag,
					 uint64_t expectedResults, Adventure& adventure) {
	uint64_t result=adventure.packEggs(eggs, bag);
	assert_eq_msg(result, expectedResults, "Unexpected packing result");
}

void testCase1(Adventure& adventure) {
	std::vector<Egg> eggs1{Egg(1, 1), Egg(2, 2), Egg(3, 3)};
	for(int i=4; i<10; ++i) {
		correctnessTest(eggs1, BottomlessBag(i), std::min(i, 6), adventure);
	}
}

void testCase2(Adventure& adventure) {
	std::vector<Egg> eggs2{Egg(5, 99999), Egg(1, 1), Egg(2, 2), Egg(3, 3),
						   Egg(1, 99999)};
	correctnessTest(eggs2, BottomlessBag(1), 99999, adventure);
	correctnessTest(eggs2, BottomlessBag(3), 99999+2, adventure);
	correctnessTest(eggs2, BottomlessBag(5), 99999+4, adventure);
	correctnessTest(eggs2, BottomlessBag(6), 2*99999, adventure);
}

void testCase3(Adventure& adventure) {
	std::vector<Egg> eggs;
	eggs.reserve(33);
	for(int i=0; i<33; ++i) {
		eggs.emplace_back(i, i*i+7);
	}

	correctnessTest(eggs, BottomlessBag(100), 2969, adventure);
}

void testCase4(Adventure& adventure) {
	std::vector<Egg> eggs;
	for(int i=0; i<100; ++i) {
		eggs.push_back(Egg(i%10, i*3+2));
	}

	correctnessTest(eggs, BottomlessBag(10000), 15050, adventure);
}

// This test may not parallelize well. Why?
void testCase5(Adventure& adventure) {
	std::vector<Egg> eggs;
	for(int i=0; i<700; ++i) {
		eggs.push_back(Egg(i, i*5+33));
	}

	correctnessTest(eggs, BottomlessBag(2000), 12079, adventure);
}

void genTestCase6() {
	for(int j=0; j<1000; j++)
		test6_eggs.push_back(Egg(j%5+1, j%(10)));

	std::shared_ptr<Adventure> adv=std::shared_ptr<Adventure>(new LonesomeAdventure{});
	ans=adv->packEggs(test6_eggs, test6_bag);
}

void testCase6(Adventure& adventure) {
	correctnessTest(test6_eggs, test6_bag, ans, adventure);
}

int main(int argc, char** argv) {
	genTestCase6();
	std::cout<<"Starting\n";
	for(std::shared_ptr<Adventure> adventure :
			std::vector<std::shared_ptr<Adventure>>{
					std::shared_ptr<Adventure>(new LonesomeAdventure{}),
					std::shared_ptr<Adventure>(new TeamAdventure(1)),
					std::shared_ptr<Adventure>(new TeamAdventure(2)),
					std::shared_ptr<Adventure>(new TeamAdventure(3)),
					std::shared_ptr<Adventure>(new TeamAdventure(4)),

			}) {
		std::cout<<adventure->advType();
		double min=ULONG_LONG_MAX;
		double max=0;
		double sum=0;
		for(int i=1; i<=TRIES; i++) {
			double run=runAndReturnDuration([&adventure]() {
				testCase6(*adventure);
			});
			max=std::max(run, max);
			min=std::min(run, min);
			sum+=run;
		}
		double avg=sum/TRIES;
		std::cout<<","<<int(max)<<","<<int(min)<<","<<int(avg);
		std::cout<<std::endl;
	}
	return 0;
}
