module Hw2 where

import Data.List -- YOU MAY USE THIS MODULE FOR SORTING THE AGENTS

data Level = Newbie | Intermediate | Expert deriving (Enum, Eq, Ord, Show, Read)
data Hunter = Hunter {hID::Int, hlevel::Level, hEnergy::Int, hNumberOfCatches::Int, hActions::[Direction]} deriving (Eq, Show, Read)
data Prey = Prey {pID::Int, pEnergy::Int, pActions::[Direction]} deriving (Eq, Show, Read)
data Cell = O | X | H Hunter | P Prey | T deriving (Eq, Show, Read)
data Direction = N | S | E | W deriving (Eq, Show, Read)
type Coordinate = (Int, Int)
-- DO NOT CHANGE THE DEFINITIONS ABOVE. -- 

-- INSTANCES OF Ord FOR SORTING, UNCOMMENT AND COMPLETE THE IMPLEMENTATIONS --
instance Ord Hunter where
   compare h1 h2   	| (hlevel h1) == (hlevel h2) = if (compare (hEnergy h1) (hEnergy h2) /= EQ)
 														then compare (hEnergy h1) (hEnergy h2)
 														else if (compare (hNumberOfCatches h1) (hNumberOfCatches h2) /= EQ)
	 														then compare (hNumberOfCatches h1) (hNumberOfCatches h2)
	 														else compare (hID h2) (hID h1) 
	 				| (hlevel h1) == Newbie = LT
	 				| (hlevel h1) == Expert = GT
	 				| (hlevel h1) == Intermediate = if (hlevel h2) == Newbie
	 												then GT
	 												else LT
instance Ord Prey where
   compare p1 p2 = if (compare (pEnergy p1) (pEnergy p2) /= EQ)
						then compare (pEnergy p1) (pEnergy p2)
						else compare (pID p2) (pID p1)
   												

conc [] = []
conc (first:rest) = first ++ conc rest

check x | x==O = 2
		| x==T = 2
		| x==X = 2

check x = case x of 
	P (Prey _ _ _) -> 1
	otherwise -> 0

direc_hunter (Hunter _ _ _ _ x ) = x
level_hunter (Hunter _ x _ _ _ ) = x
energy_hunter (Hunter _ _ x _ _ ) = x
id_hunter (Hunter x _ _ _ _ ) = x
catches_hunter (Hunter _ _ _ x _ ) = x
h_s(H x)=x

direc_prey (Prey _ _ x ) = x
energy_prey (Prey _ x _ ) = x
id_prey (Prey x _ _ ) = x
p_s (P x)=x

groupingAgents x [] index list_of_prey list_of_hunter  = (list_of_prey,list_of_hunter)
groupingAgents x (first:rest) index list_of_prey list_of_hunter  = if check(first)==1 
																		then  groupingAgents x rest (index+1) (((p_s first),(mod index x, div index x)):list_of_prey) list_of_hunter
																		else if check(first)==0
																			then groupingAgents x rest (index+1) list_of_prey (((h_s first),(mod index x, div index x)):list_of_hunter)
																			else groupingAgents x rest (index+1) list_of_prey list_of_hunter

checkPoint list = groupingAgents (length(list!!0)) (conc list) 0 [] [] 

moveHunter list [] movedList = movedList
moveHunter list (sor_f:sor_r) movedList =if (energy_hunter (fst sor_f))<10 
											then 
												let 
													coord_hunter = (snd sor_f)
													h1 = (Hunter (id_hunter (fst sor_f)) (level_hunter (fst sor_f)) ((energy_hunter (fst sor_f))+1) (catches_hunter (fst sor_f)) (actions (direc_hunter (fst sor_f))))
													in [(h1,coord_hunter)] ++ movedList ++ moveHunter list sor_r movedList
												else 
													let 
														coord_hunter = finalCoord (newCoord (snd sor_f) list (head (direc_hunter (fst sor_f)))) (snd (sor_f)) list
														h1 = (Hunter (id_hunter (fst sor_f)) (level_hunter (fst sor_f)) ((energy_hunter (fst sor_f))-1) (catches_hunter (fst sor_f)) (actions (direc_hunter (fst sor_f))))
														in [(h1,coord_hunter)] ++ movedList ++ moveHunter list sor_r movedList

movePrey list [] movedList = movedList
movePrey list (sor_f:sor_r) movedList = if (energy_prey (fst sor_f))<10 
											then
												let 
													coord_prey = (snd sor_f)
													p1 = (Prey (id_prey (fst sor_f)) (is_in_trap (energy_prey (fst sor_f)) list coord_prey) (actions (direc_prey (fst sor_f))))
													in [(p1,coord_prey)] ++ movedList ++ movePrey list sor_r movedList
											else 	
												let 
													coord_prey = finalCoord (newCoord (snd sor_f) list (head(direc_prey (fst sor_f)))) (snd sor_f) list
													p1 = (Prey (id_prey (fst sor_f)) (energy_prey_trap (energy_prey (fst sor_f)) list coord_prey) (actions (direc_prey (fst sor_f))))
													in [(p1,coord_prey)] ++ movedList ++ movePrey list sor_r movedList


newCoord :: (Int,Int) -> [[Cell]] -> Direction -> (Int,Int)
newCoord tuple list direction | direction==W && (fst tuple) == 0 = tuple
							  | direction==N && (snd tuple) == 0 = tuple
							  | direction==E && (fst tuple) == (length(list!!0)-1) = tuple
							  | direction==S && (snd tuple) == (length(list)-1) = tuple						
						   	  | otherwise = newCoordHelper tuple direction
								
newCoordHelper :: (Int,Int) -> Direction -> (Int,Int)
newCoordHelper tuple direction | direction==W = ((fst tuple)-1,(snd tuple))
							   | direction==N = ((fst tuple),(snd tuple)-1)
							   | direction==E = ((fst tuple)+1,(snd tuple))
							   | direction==S = ((fst tuple),(snd tuple)+1)
							   | otherwise = tuple

finalCoord :: (Int,Int) -> (Int,Int) -> [[Cell]] -> (Int,Int) 
finalCoord newTuple tuple list = if list!!(snd newTuple)!!(fst newTuple)==X
									then tuple
								else  newTuple

actions (first:rest)=rest

energy_prey_trap energy list coord = if list!!(snd coord)!!(fst coord) == T 
										then if (energy-10) < 0
											then 0
											else (energy-11)
									else (energy-1)

is_in_trap energy list coord = if list!!(snd coord)!!(fst coord) == T 
									then 0
									else (energy+1)

checkPoint_two list = ((moveHunter list (snd (checkPoint list)) [] ),(movePrey list (fst (checkPoint list)) []))
checkPoint_two2 list hunter prey =((moveHunter list hunter [] ),(movePrey list prey []))


hunterCatches hunter [] count updatedPreys = (count,updatedPreys)
hunterCatches hunter (p_first:p_last) count updatedPreys = if count < 2
																then  
																	if (snd hunter) == (snd p_first) 
																		then hunterCatches hunter p_last (count+1) updatedPreys
																		else hunterCatches hunter p_last count ([p_first]++updatedPreys) 
															else (count,(p_first:p_last)++updatedPreys)

upDate_hunters [] preylist hunters_after_actions = (hunters_after_actions,preylist)															
upDate_hunters (h_first:h_last) preylist hunters_after_actions = 
	if  fst (hunterCatches h_first preylist 0 []) > 0 
		then 
			let 
				h1 = (Hunter (id_hunter (fst h_first)) (level_hunter (fst h_first)) ( increase_en_catch (fst (hunterCatches h_first preylist 0 [])) (energy_hunter (fst h_first)) ) ((catches_hunter (fst h_first))+(fst (hunterCatches h_first preylist 0 []))) (direc_hunter (fst h_first)))
				in upDate_hunters h_last (snd (hunterCatches h_first preylist 0 [])) ([(h1,(snd h_first))] ++ hunters_after_actions)
		else upDate_hunters h_last preylist ([h_first] ++ hunters_after_actions)

increase_en_catch count energy = if energy > 100 
									then 100
									else if (energy+(count*20)) > 100
										then 100
										else (energy+(count*20))

simulate:: [[Cell]]-> ([(Hunter, Coordinate)],[(Prey,Coordinate)])
simulate list = all_actions list (reverse ( sortBy (\(a, _) (b, _) -> compare a b) (snd (checkPoint list)) )) ( sortBy (\(a, _) (b, _) -> compare a b) (fst (checkPoint list))) 


all_actions list list_of_hunter [] = (list_of_hunter,[])
all_actions list list_of_hunter list_of_prey = if direc_hunter (fst (head list_of_hunter))==[] 
													then (list_of_hunter, list_of_prey)
													else let 

															list_of_hunter2 = reverse (sortBy (\(a, _) (b, _) -> compare a b) (fst (upDate_hunters (moveHunter list list_of_hunter []) (movePrey list list_of_prey []) [])))
															list_of_prey2 =  sortBy (\(a, _) (b, _) -> compare a b) (snd (upDate_hunters (moveHunter list list_of_hunter []) (movePrey list list_of_prey []) [])) 
															
															in all_actions list list_of_hunter2 list_of_prey2 

checkPoint_three list = upDate_hunters (reverse (sort (fst (checkPoint_two list)))) (sort (snd (checkPoint_two list))) []


