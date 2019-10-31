module Hw1 where

data Cell = H | P | O | X deriving (Read,Show,Eq)
data Result = Fail | Caught (Int,Int) deriving (Read,Show,Eq)
data Direction = N | S | E | W deriving (Read,Show,Eq)

simulate :: [[Cell]] -> [(Direction, Direction)] -> Result
-- DO NOT CHANGE ABOVE THIS LINE, WRITE YOUR CODE BELOW --			
simulate list direction = let{
							hunter=findIndex list H ;
							hunter1 = findIndexHelper hunter list;

							prey = findIndex list P ;
							prey1 = findIndexHelper prey list;}
								in simulateHelper list direction hunter1 prey1
							
simulateHelper :: [[Cell]] -> [(Direction, Direction)] ->(Int,Int)->(Int,Int)-> Result								
simulateHelper list [] hunter prey= Fail
simulateHelper list (first:rest) hunter prey = let{
												hunter1 = newCoord hunter list (fst first);
												hunter2 = finalCoord hunter1 hunter list;

												prey1 = newCoord prey list (snd first);
												prey2 = finalCoord prey1 prey list;
												}
													in if hunter2==prey2
															then Caught hunter2
															else simulateHelper list rest hunter2 prey2

findIndex :: [[Cell]] -> Cell -> Int
findIndex [] look = -1
findIndex (first:rest) look = findIndex2 (first:rest) first look
							 
findIndex2 :: [[Cell]] -> [Cell] -> Cell -> Int
findIndex2 (bigfirst:bigScond) [] look = findIndex bigScond look
findIndex2 (bigfirst:bigScond) (first:rest) look = 	
				if first == look
					then 0
				else 1+findIndex2 (bigfirst:bigScond) rest look 

findIndexHelper :: Int -> [[Cell]] -> (Int,Int) 
findIndexHelper total list = let 
								x=length(list!!0)
							in ((mod total x),(div total x))
			
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
