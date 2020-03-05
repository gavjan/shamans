#include <vector>
#include <climits>

#include "../adventure.h"
#include "../utils.h"

#define TRIES 5

void runAndVerify(Adventure& adventure, std::vector<GrainOfSand>& grains,
				  std::vector<GrainOfSand>& result) {
	adventure.arrangeSand(grains);
	assert_msg(grains == result, "Wrong sand arrangement");
}

void testCase1(Adventure& adventure) {
	std::vector<GrainOfSand> t1={GrainOfSand(3), GrainOfSand(2),
								 GrainOfSand(1)};
	std::vector<GrainOfSand> r1={GrainOfSand(1), GrainOfSand(2),
								 GrainOfSand(3)};
	runAndVerify(adventure, t1, r1);
	std::vector<GrainOfSand> t2={GrainOfSand(5), GrainOfSand(3), GrainOfSand(2),
								 GrainOfSand(1), GrainOfSand(4)};
	std::vector<GrainOfSand> r2={GrainOfSand(1), GrainOfSand(2), GrainOfSand(3),
								 GrainOfSand(4), GrainOfSand(5)};
	runAndVerify(adventure, t2, r2);
	std::vector<GrainOfSand> t3={GrainOfSand(7), GrainOfSand(7), GrainOfSand(7),
								 GrainOfSand(1), GrainOfSand(1), GrainOfSand(4),
								 GrainOfSand(5)};
	std::vector<GrainOfSand> r3={GrainOfSand(1), GrainOfSand(1), GrainOfSand(4),
								 GrainOfSand(5), GrainOfSand(7), GrainOfSand(7),
								 GrainOfSand(7)};
	runAndVerify(adventure, t3, r3);
}

int main(int argc, char** argv) {
	int BIG=1000000;
	std::vector<GrainOfSand> big(BIG);
	std::generate(big.begin(), big.end(), std::rand);
	std::vector<GrainOfSand> ans_big=big;
	std::sort(ans_big.begin(), ans_big.end());
	for(std::shared_ptr<Adventure> adventure :
			std::vector<std::shared_ptr<Adventure>>{
					std::shared_ptr<Adventure>(new LonesomeAdventure{}),
					std::shared_ptr<Adventure>(new TeamAdventure(1)),
					std::shared_ptr<Adventure>(new TeamAdventure(2)),
					std::shared_ptr<Adventure>(new TeamAdventure(3)),
					std::shared_ptr<Adventure>(new TeamAdventure(4))
			}) {
		std::cout<<adventure->advType();
		double min=ULONG_LONG_MAX;
		double max=0;
		double sum=0;
		for(int i=1; i<=TRIES; i++) {
			auto t3=big;
			auto r3=ans_big;
			double run=runAndReturnDuration([&adventure, &t3, &r3]() {
				runAndVerify(*adventure, t3, r3);
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
