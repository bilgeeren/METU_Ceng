fib 1 = 1
fib 2 = 1
fib n = fib (n-1) + fib (n-2)

fibx (0,v1,v2) = v2
fibx (n,v1,v2) = fibx (n-1,v2 ,v1+v2)

fiby (n,v1,v2) | n==0 = v2
			   | otherwise = fiby(n-1,v2,v1+v2)


-- local definitions
 
ff x = let {s = x+1;c = x+2} in 2 * s * c

gg x = let {f x = x*x ; y=4} 
	in (let {f x = x+ 1;z=3;x=10} 
		in(let {y=2;x=3} 
			in f x * y +z))


pow x 0 = 1
pow x y = x * pow x (y-1)

pow2 (x,0) = 1
pow2 (x,y) | y `mod` 2==0 = pow2 (x,y `div` 2) * pow2 (x,y `div` 2)
		   | otherwise = x * pow2 (x,y-1) 

pow1 (x,0) = 1
pow1 (x,y) | y `mod` 2==0 = pw* pw
		   | otherwise = x * pw*pw 
		    where pw = pow1(x,y `div` 2)


pow3 (x,0) = 1
pow3 (x,y) = let{pw = pow3(x,y `div` 2)}
			 in(if y `mod` 2==0 then pw*pw
			 					else x*pw*pw)
		       


--User Defined Types

type Person = (String,Integer,Bool)
data WState = Sunny | PartClouds 

data Values = Rat (Integer,Integer) | RV Integer deriving Show


add (Rat(a,b),Rat(c,d)) = Rat(a+b,c+d)


add2 val = case val of
			Rat(a,b) -> a+b
			RV v -> v
			



fff (x,y,z) = z 


--list functions

newLength [] = 0
newLength (_:r) = 1+ newLength r

data Result a = Success a | Failure deriving Show


ifexists [] _ _ = Failure
ifexists (s:r) t n | t == s = Success n
				   | otherwise = ifexists r t (n+1)

ifexists1 [] _ _ = Nothing
ifexists1 (s:r) t n | t == s = Just n
				   | otherwise = ifexists1 r t (n+1)


newReverse [] lst= lst
newReverse (s:r) lst = newReverse r (s:lst)

addFront lst n = (n:lst) 



isPrefix [] _ = True
isPrefix _ [] = False

isPrefix (a:b) (c:d)= if a==c then isPrefix b d
							 else False



isSubstring _ [] = False
isSubstring a (c:d) = if isPrefix a (c:d) then True
											else isSubstring a d  


data Tree a = Etree | Node a (Tree a) (Tree a) deriving Show


insert Etree v = Node v Etree Etree
insert (Node a left right) v | a== v = (Node v left right)
							 | v<a = Node a (insert left v) right
							 | otherwise = Node a left (insert right v)


findMin (Node a Etree _) = a
findMin (Node a left right) = findMin left

findMin (Node a _ Etree) = a
findMin (Node a left right) = findMin right

traverse_ Etree = []
traverse_ (Node a left right) = (traverse_ left) ++ [a] ++ (traverse_ right)


leaves_ Etree = []
leaves_ (Node a Etree Etree) = [a]
leaves_ (Node a left right) =(leaves_ left)++ (leaves_ right)



get Etree _ = Nothing
get (Node a left right) v | v==a = Just (Node a left right)
						  | v<a = get left v 
						  | otherwise = get right v



data HashTree a b = EHash | NodeHash a b (HashTree a b) (HashTree a b) deriving Show


insertHash EHash x y = NodeHash x y EHash EHash
insertHash (NodeHash a b left right) x y | x==a = NodeHash x y left right
										 | x<a = NodeHash a b (insertHash left x y) right
										 | otherwise = NodeHash a b left (insertHash right x y)


getHash EHash x = Nothing
getHash (NodeHash a b left right) x | x==a = Just b
									| x<a = getHash left x
									| otherwise = getHash right x


pathHash EHash x = []
pathHash (NodeHash a b left right) x | x==a = [(a,b)]
									 | x<a = [(a,b)] ++ pathHash left x
									 | otherwise = [(a,b)] ++ pathHash right x



inrange_ Etree _ _ = []
inrange_ (Node a left right) lower upper | a>upper = (inrange_ left lower upper)
										 | a<lower = (inrange_ right lower upper) 
										 | otherwise = (inrange_ left lower upper) ++ [a] ++ (inrange_ right lower upper)




