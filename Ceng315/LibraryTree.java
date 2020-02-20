import java.util.ArrayList;
public class LibraryTree {
	
	public LibraryNode primaryRoot;		//root of the primary B+ tree
	public LibraryNode secondaryRoot;	//root of the secondary B+ tree
	public LibraryTree(Integer order)
	{
		LibraryNode.order = order;
		primaryRoot = new LibraryNodeLeaf(null);
		primaryRoot.level = 0;
		secondaryRoot = new LibraryNodeLeaf(null);
		secondaryRoot.level = 0;
	}
	
	public void addBook(CengBook book) {		
		int ord = LibraryNode.order;
		LibraryNode node = findNode_prim(book.key(),primaryRoot);
		LibraryNode node2 = findNode_sec(book.year(),secondaryRoot);

		//Primary
		int i;
		for (i=0;i<((LibraryNodeLeaf) node).bookCount();i++){
			if(((LibraryNodeLeaf) node).bookKeyAtIndex(i) > book.key()){
				break;
			}
		}
		((LibraryNodeLeaf) node).addBook(i,book);
		

		if(((LibraryNodeLeaf) node).bookCount() > ord*2){
			addHelper_prim(node,ord);
		}

		//Secondary 
		for (i=0;i<((LibraryNodeLeaf) node2).bookCount();i++){
			if(((LibraryNodeLeaf) node2).bookYearAtIndex(i) > book.year()){
				break;
			}
			else if(((LibraryNodeLeaf) node2).bookYearAtIndex(i) == book.year() && ((LibraryNodeLeaf) node2).bookKeyAtIndex(i) > book.key()){
				break;
			}
		}
		((LibraryNodeLeaf) node2).addBook(i,book);
		

		if(((LibraryNodeLeaf) node2).bookCount() > ord*2){

			addHelper_sec(node2,ord);
		}
	}
	
	public void addHelper_prim(LibraryNode node,int index){
		int ord = LibraryNode.order;
		int lev = node.level;
		if (node.type == LibraryNodeType.Leaf){
			if (node.getParent() == null){
				//System.out.println("1.1 "+((LibraryNodeLeaf) node).bookCount());
				LibraryNodePrimaryIndex parent = new LibraryNodePrimaryIndex(null);
				LibraryNodeLeaf left = new LibraryNodeLeaf(parent);
				LibraryNodeLeaf right = new LibraryNodeLeaf(parent);
				int snd =0;
				for (int i=0;i<((LibraryNodeLeaf) node).bookCount();i++){
					if(i<index){
						left.addBook(i,((LibraryNodeLeaf) node).bookAtIndex(i));
					}else{
						right.addBook(snd,((LibraryNodeLeaf) node).bookAtIndex(i));
						snd++;
					}
				}
				parent.getKeys().add(0,right.bookAtIndex(0).key());
				parent.getAllChildren().add(left);
				parent.getAllChildren().add(right);
				left.setParent(parent);
				right.setParent(parent);
				parent.level = lev+1;
				primaryRoot = parent;
				node = null;
			}
			else{
				//System.out.println("1.2 "+((LibraryNodeLeaf) node).bookCount());
				LibraryNode parent = node.getParent();
				LibraryNodeLeaf left = new LibraryNodeLeaf(parent);
				LibraryNodeLeaf right = new LibraryNodeLeaf(parent);
				int snd =0;
				for (int i=0;i<((LibraryNodeLeaf) node).bookCount();i++){
					if(i<index){
						left.addBook(i,((LibraryNodeLeaf) node).bookAtIndex(i));
					}else{
						right.addBook(snd,((LibraryNodeLeaf) node).bookAtIndex(i));
						snd++;
					}
				}
				int i=0;
				for(i=0;i<((LibraryNodePrimaryIndex)parent).keyCount();i++){
					if(((LibraryNodePrimaryIndex)parent).keyAtIndex(i)>right.bookKeyAtIndex(0)){
						break;
					}
				}
				((LibraryNodePrimaryIndex)parent).getKeys().add(i,right.bookKeyAtIndex(0));
				((LibraryNodePrimaryIndex)parent).getAllChildren().remove(i);
				((LibraryNodePrimaryIndex)parent).getAllChildren().add(i,left);
				((LibraryNodePrimaryIndex)parent).getAllChildren().add(i+1,right);
				left.setParent(parent);
				right.setParent(parent);
				node = null;

		
				if(((LibraryNodePrimaryIndex)parent).keyCount() > ord*2){
					//System.out.println("parent helpera ");
					addHelper_prim(parent,ord);
				}
				
			}	
		}
		else {
			
			if( node.getParent()==null){
				//System.out.println("2.1 "+((LibraryNodePrimaryIndex)node).keyCount());
				LibraryNodePrimaryIndex parent = new LibraryNodePrimaryIndex(null);
				LibraryNodePrimaryIndex left = new LibraryNodePrimaryIndex(null);
				LibraryNodePrimaryIndex right = new LibraryNodePrimaryIndex(null);
				int snd =0;
				int i;

				/*for (i=0;i<((LibraryNodePrimaryIndex)node).keyCount();i++){
					System.out.println("node"+((LibraryNodePrimaryIndex)node).keyAtIndex(i));
				}*/
				for (i=0;i<((LibraryNodePrimaryIndex)node).keyCount();i++){
					if(i<index){
						//System.out.println("left "+((LibraryNodePrimaryIndex)node).keyAtIndex(i));
						left.getKeys().add(i,((LibraryNodePrimaryIndex)node).keyAtIndex(i));
						left.getAllChildren().add(i,((LibraryNodePrimaryIndex)node).getChildrenAt(i));
						((LibraryNodePrimaryIndex)node).getChildrenAt(i).setParent(left);
					}
					else if (i == index){
						left.getAllChildren().add(i,((LibraryNodePrimaryIndex)node).getChildrenAt(i));
						((LibraryNodePrimaryIndex)node).getChildrenAt(i).setParent(left);
					}
					else{
						//System.out.println("right "+((LibraryNodePrimaryIndex)node).keyAtIndex(i));
						right.getKeys().add(snd,((LibraryNodePrimaryIndex)node).keyAtIndex(i));
						right.getAllChildren().add(snd,((LibraryNodePrimaryIndex)node).getChildrenAt(i));
						((LibraryNodePrimaryIndex)node).getChildrenAt(i).setParent(right);
						snd++;
					}
				}
				right.getAllChildren().add(snd,((LibraryNodePrimaryIndex)node).getChildrenAt(i));
				((LibraryNodePrimaryIndex)node).getChildrenAt(i).setParent(right);

				parent.getKeys().add(0,((LibraryNodePrimaryIndex)node).keyAtIndex(index));
				parent.getAllChildren().add(left);
				parent.getAllChildren().add(right);
				left.setParent(parent);
				right.setParent(parent);
				parent.level = lev+1;
				primaryRoot = parent;
				/*for (i=0;i<((LibraryNodePrimaryIndex)parent).keyCount();i++){
					System.out.println("parent"+((LibraryNodePrimaryIndex)parent).keyAtIndex(i));
				}*/
				node = null;

			}
			else {
				//System.out.println("2.2 "+((LibraryNodePrimaryIndex)node).keyCount());
				LibraryNode parent = node.getParent();
				LibraryNodePrimaryIndex left = new LibraryNodePrimaryIndex(null);
				LibraryNodePrimaryIndex right = new LibraryNodePrimaryIndex(null);
				int snd =0;
				int i;
				for (i=0;i<((LibraryNodePrimaryIndex)node).keyCount();i++){
					if(i<index){
						//System.out.println("left "+((LibraryNodePrimaryIndex)node).keyAtIndex(i));
						left.getKeys().add(i,((LibraryNodePrimaryIndex)node).keyAtIndex(i));
						left.getAllChildren().add(i,((LibraryNodePrimaryIndex)node).getChildrenAt(i));
						((LibraryNodePrimaryIndex)node).getChildrenAt(i).setParent(left);
					}
					else if (i == index){
						left.getAllChildren().add(i,((LibraryNodePrimaryIndex)node).getChildrenAt(i));
						((LibraryNodePrimaryIndex)node).getChildrenAt(i).setParent(left);
						
					}
					else{
						//System.out.println("right "+((LibraryNodePrimaryIndex)node).keyAtIndex(i));
						right.getKeys().add(snd,((LibraryNodePrimaryIndex)node).keyAtIndex(i));
						right.getAllChildren().add(snd,((LibraryNodePrimaryIndex)node).getChildrenAt(i));
						((LibraryNodePrimaryIndex)node).getChildrenAt(i).setParent(right);
						snd++;
					}
				}
				right.getAllChildren().add(snd,((LibraryNodePrimaryIndex)node).getChildrenAt(i));
				((LibraryNodePrimaryIndex)node).getChildrenAt(i).setParent(right);

				for(i=0;i<((LibraryNodePrimaryIndex)parent).keyCount();i++){
					if(((LibraryNodePrimaryIndex)parent).keyAtIndex(i)>((LibraryNodePrimaryIndex)node).keyAtIndex(index)){
						
						break;
					}
				}
				((LibraryNodePrimaryIndex)parent).getKeys().add(i,((LibraryNodePrimaryIndex)node).keyAtIndex(index));
				((LibraryNodePrimaryIndex)parent).getAllChildren().remove(i);
				((LibraryNodePrimaryIndex)parent).getAllChildren().add(i,left);
				((LibraryNodePrimaryIndex)parent).getAllChildren().add(i+1,right);
				node = null;
				left.setParent(parent);
				right.setParent(parent);
				
				if(((LibraryNodePrimaryIndex)parent).keyCount() > ord*2){
					addHelper_prim(parent,ord);
				}
				
			}
		}

	}

	public void addHelper_sec(LibraryNode node,int index){
		int ord = LibraryNode.order;
		int lev = node.level;
		if (node.type == LibraryNodeType.Leaf){
			if (node.getParent() == null){
				//System.out.println("1.1 "+((LibraryNodeLeaf) node).bookCount());
				LibraryNodeSecondaryIndex parent = new LibraryNodeSecondaryIndex(null);
				LibraryNodeLeaf left = new LibraryNodeLeaf(parent);
				LibraryNodeLeaf right = new LibraryNodeLeaf(parent);
				int snd =0;
				for (int i=0;i<((LibraryNodeLeaf) node).bookCount();i++){
					if(i<index){
						left.addBook(i,((LibraryNodeLeaf) node).bookAtIndex(i));
					}else{
						right.addBook(snd,((LibraryNodeLeaf) node).bookAtIndex(i));
						snd++;
					}
				}
				parent.getKeys().add(0,right.bookAtIndex(0).key());
				parent.getYears().add(0,right.bookAtIndex(0).year());
				parent.getAllChildren().add(left);
				parent.getAllChildren().add(right);
				left.setParent(parent);
				right.setParent(parent);
				parent.level = lev+1;
				secondaryRoot = parent;
				node = null;
			}
			else{
				//System.out.println("1.2 "+((LibraryNodeLeaf) node).bookCount());
				LibraryNode parent = node.getParent();
				LibraryNodeLeaf left = new LibraryNodeLeaf(parent);
				LibraryNodeLeaf right = new LibraryNodeLeaf(parent);
				int snd =0;
				for (int i=0;i<((LibraryNodeLeaf) node).bookCount();i++){
					if(i<index){
						left.addBook(i,((LibraryNodeLeaf) node).bookAtIndex(i));
					}else{
						right.addBook(snd,((LibraryNodeLeaf) node).bookAtIndex(i));
						snd++;
					}
				}
				int i=0;
				for(i=0;i<((LibraryNodeSecondaryIndex)parent).keyCount();i++){
					if(((LibraryNodeSecondaryIndex)parent).yearAtIndex(i)>right.bookYearAtIndex(0)){
						break;
					}
					else if (((LibraryNodeSecondaryIndex)parent).yearAtIndex(i) == right.bookYearAtIndex(0) && ((LibraryNodeSecondaryIndex)parent).keyAtIndex(i)>right.bookKeyAtIndex(0)){
						break;
					}
				}

				((LibraryNodeSecondaryIndex)parent).getKeys().add(i,right.bookKeyAtIndex(0));
				((LibraryNodeSecondaryIndex)parent).getYears().add(i,right.bookYearAtIndex(0));
				
				((LibraryNodeSecondaryIndex)parent).getAllChildren().remove(i);
				((LibraryNodeSecondaryIndex)parent).getAllChildren().add(i,left);
				((LibraryNodeSecondaryIndex)parent).getAllChildren().add(i+1,right);
				left.setParent(parent);
				right.setParent(parent);
				node = null;

		
				if(((LibraryNodeSecondaryIndex)parent).keyCount() > ord*2){
					//System.out.println("parent helpera ");
					addHelper_sec(parent,ord);
				}
				
			}	
		}
		else{
			
			if( node.getParent()==null){
				//System.out.println("2.1 "+((LibraryNodeSecondaryIndex)node).keyCount());
				LibraryNodeSecondaryIndex parent = new LibraryNodeSecondaryIndex(null);
				LibraryNodeSecondaryIndex left = new LibraryNodeSecondaryIndex(null);
				LibraryNodeSecondaryIndex right = new LibraryNodeSecondaryIndex(null);
				int snd =0;
				int i;

				/*for (i=0;i<((LibraryNodeSecondaryIndex)node).keyCount();i++){
					System.out.println("node"+((LibraryNodeSecondaryIndex)node).keyAtIndex(i));
				}*/
				for (i=0;i<((LibraryNodeSecondaryIndex)node).keyCount();i++){
					if(i<index){
						//System.out.println("left "+((LibraryNodeSecondaryIndex)node).keyAtIndex(i));
						left.getKeys().add(i,((LibraryNodeSecondaryIndex)node).keyAtIndex(i));
						left.getYears().add(i,((LibraryNodeSecondaryIndex)node).yearAtIndex(i));

						left.getAllChildren().add(i,((LibraryNodeSecondaryIndex)node).getChildrenAt(i));
						((LibraryNodeSecondaryIndex)node).getChildrenAt(i).setParent(left);
					}
					else if (i == index){
						left.getAllChildren().add(i,((LibraryNodeSecondaryIndex)node).getChildrenAt(i));
						((LibraryNodeSecondaryIndex)node).getChildrenAt(i).setParent(left);
					}
					else{
						//System.out.println("right "+((LibraryNodeSecondaryIndex)node).keyAtIndex(i));
						right.getKeys().add(snd,((LibraryNodeSecondaryIndex)node).keyAtIndex(i));
						right.getYears().add(snd,((LibraryNodeSecondaryIndex)node).yearAtIndex(i));

						right.getAllChildren().add(snd,((LibraryNodeSecondaryIndex)node).getChildrenAt(i));
						((LibraryNodeSecondaryIndex)node).getChildrenAt(i).setParent(right);
						snd++;
					}
				}
				right.getAllChildren().add(snd,((LibraryNodeSecondaryIndex)node).getChildrenAt(i));
				((LibraryNodeSecondaryIndex)node).getChildrenAt(i).setParent(right);

				parent.getKeys().add(0,((LibraryNodeSecondaryIndex)node).keyAtIndex(index));
				parent.getYears().add(0,((LibraryNodeSecondaryIndex)node).yearAtIndex(index));

				parent.getAllChildren().add(left);
				parent.getAllChildren().add(right);
				left.setParent(parent);
				right.setParent(parent);
				parent.level = lev+1;
				secondaryRoot = parent;
				/*for (i=0;i<((LibraryNodeSecondaryIndex)parent).keyCount();i++){
					System.out.println("parent"+((LibraryNodeSecondaryIndex)parent).keyAtIndex(i));
				}*/
				node = null;

			}
			else {
				//System.out.println("2.2 "+((LibraryNodeSecondaryIndex)node).keyCount());
				
				LibraryNode parent = node.getParent();
				/*for (int i=0;i<((LibraryNodeSecondaryIndex)parent).keyCount();i++){
					System.out.println("parent "+((LibraryNodeSecondaryIndex)parent).yearAtIndex(i));
				}*/
				LibraryNodeSecondaryIndex left = new LibraryNodeSecondaryIndex(null);
				LibraryNodeSecondaryIndex right = new LibraryNodeSecondaryIndex(null);
				int snd =0;
				int i;
				for (i=0;i<((LibraryNodeSecondaryIndex)node).keyCount();i++){
					if(i<index){
						//System.out.println("left "+((LibraryNodeSecondaryIndex)node).keyAtIndex(i));
						left.getKeys().add(i,((LibraryNodeSecondaryIndex)node).keyAtIndex(i));
						left.getYears().add(i,((LibraryNodeSecondaryIndex)node).yearAtIndex(i));

						left.getAllChildren().add(i,((LibraryNodeSecondaryIndex)node).getChildrenAt(i));
						((LibraryNodeSecondaryIndex)node).getChildrenAt(i).setParent(left);
					}
					else if (i == index){
						left.getAllChildren().add(i,((LibraryNodeSecondaryIndex)node).getChildrenAt(i));
						((LibraryNodeSecondaryIndex)node).getChildrenAt(i).setParent(left);

					}
					else{
						//System.out.println("right "+((LibraryNodeSecondaryIndex)node).keyAtIndex(i));
						right.getKeys().add(snd,((LibraryNodeSecondaryIndex)node).keyAtIndex(i));
						right.getYears().add(snd,((LibraryNodeSecondaryIndex)node).yearAtIndex(i));

						right.getAllChildren().add(snd,((LibraryNodeSecondaryIndex)node).getChildrenAt(i));
						((LibraryNodeSecondaryIndex)node).getChildrenAt(i).setParent(right);
						snd++;
					}
				}
				right.getAllChildren().add(snd,((LibraryNodeSecondaryIndex)node).getChildrenAt(i));
				((LibraryNodeSecondaryIndex)node).getChildrenAt(i).setParent(right);

				for(i=0;i<((LibraryNodeSecondaryIndex)parent).keyCount();i++){
					if(((LibraryNodeSecondaryIndex)parent).yearAtIndex(i)>right.yearAtIndex(0)){
						break;
					}
					else if (((LibraryNodeSecondaryIndex)parent).yearAtIndex(i) == right.yearAtIndex(0) && ((LibraryNodeSecondaryIndex)parent).keyAtIndex(i)>right.keyAtIndex(0)){
						break;
					}
				}
				((LibraryNodeSecondaryIndex)parent).getKeys().add(i,((LibraryNodeSecondaryIndex)node).keyAtIndex(index));
				((LibraryNodeSecondaryIndex)parent).getYears().add(i,((LibraryNodeSecondaryIndex)node).yearAtIndex(index));
				
				((LibraryNodeSecondaryIndex)parent).getAllChildren().remove(i);
				((LibraryNodeSecondaryIndex)parent).getAllChildren().add(i,left);
				((LibraryNodeSecondaryIndex)parent).getAllChildren().add(i+1,right);
				node = null;
				left.setParent(parent);
				right.setParent(parent);
				
				if(((LibraryNodeSecondaryIndex)parent).keyCount() > ord*2){
					addHelper_sec(parent,ord);
				}
				
			}
		}

	}


	public LibraryNode findNode_prim(Integer key,LibraryNode node){
		if( node.type != LibraryNodeType.Leaf){
			int i = 0;
			for( i = 0;i<((LibraryNodePrimaryIndex)node).keyCount();i++){
				if(((LibraryNodePrimaryIndex)node).keyAtIndex(i)>key){
					return findNode_prim(key, ((LibraryNodePrimaryIndex)node).getChildrenAt(i));
				}
			}
			return findNode_prim(key, ((LibraryNodePrimaryIndex)node).getChildrenAt(i));
		}
		return node;
	}

	public LibraryNode findNode_sec(Integer key,LibraryNode node){
		if( node.type != LibraryNodeType.Leaf){
			int i = 0;
			for( i = 0;i<((LibraryNodeSecondaryIndex)node).keyCount();i++){
				if(((LibraryNodeSecondaryIndex)node).yearAtIndex(i)>key){
					return findNode_sec(key, ((LibraryNodeSecondaryIndex)node).getChildrenAt(i));
				}
			}
			return findNode_sec(key, ((LibraryNodeSecondaryIndex)node).getChildrenAt(i));
		}
		return node;
	}

	public CengBook searchBook(Integer key) {
		
		// add methods to find the book with the searched key in primary B+ tree
		// return value will not be tested, just print as the specicifications say
		LibraryNode node = recursiveSearch(key,primaryRoot);
		if(node !=null){
			for(int i =0 ;i< ((LibraryNodeLeaf)node).bookCount();i++){
				if (key == ((LibraryNodeLeaf)node).bookKeyAtIndex(i)){
					CengBook book = ((LibraryNodeLeaf)node).bookAtIndex(i);
					return book;
				}
			}
		}
	
		return null;
	}
	
	public LibraryNode recursiveSearch(Integer key,LibraryNode node){

			if( node.type != LibraryNodeType.Leaf){
			System.out.println("<index>");
			int index = 0;
			boolean flag = false;
			int i =0;
			for(i = 0;i<((LibraryNodePrimaryIndex)node).keyCount();i++){
				System.out.println(((LibraryNodePrimaryIndex)node).keyAtIndex(i));
				if(!flag && ((LibraryNodePrimaryIndex)node).keyAtIndex(i)>key){
					index = i;
					flag = true;
				}
				
			}
			//System.out.println("INDEX "+index);
			if ( flag){
				System.out.println("</index>");
				return recursiveSearch(key, ((LibraryNodePrimaryIndex)node).getChildrenAt(index));
			}
			else{
				System.out.println("</index>");
				return recursiveSearch(key, ((LibraryNodePrimaryIndex)node).getChildrenAt(i));
			}
		
		}
		int index = 0;
		boolean found =false;
		
		for(int i =0 ;i< ((LibraryNodeLeaf)node).bookCount();i++){
			if (key == ((LibraryNodeLeaf)node).bookKeyAtIndex(i)){
				index = i;
				found = true;
				break;
			}
		}
		if(found){
			System.out.println("<data>");
			CengBook book = ((LibraryNodeLeaf)node).bookAtIndex(index);
			System.out.println("<record>"+book.fullName()+"</record>");
			System.out.println("</data>");
			return node;
		}
		else{
			System.out.println("No match for "+key);
			return null;
		}
		
		
		
	}
	public void printPrimaryLibrary() {
		
		// add methods to print the primary B+ tree in Depth-first order
		print_prim_rec(primaryRoot);
		
	}

	public void print_prim_rec(LibraryNode node){

		if ( node.type != LibraryNodeType.Leaf ){
			System.out.println("<index>");
			for(int i = 0;i<((LibraryNodePrimaryIndex)node).keyCount();i++){
				System.out.println(((LibraryNodePrimaryIndex)node).keyAtIndex(i));			
			}
			System.out.println("</index>");

			for(int i=0;i<((LibraryNodePrimaryIndex)node).keyCount()+1;i++){
				print_prim_rec(((LibraryNodePrimaryIndex)node).getChildrenAt(i));
			}
		}
		else{
			System.out.println("<data>");
			for(int i =0 ;i< ((LibraryNodeLeaf)node).bookCount();i++){
				CengBook book = ((LibraryNodeLeaf)node).bookAtIndex(i);
				System.out.println("<record>"+book.fullName()+"</record>");								
			}
			System.out.println("</data>");
		}
	}
	
	public void printSecondaryLibrary() {
		
		// add methods to print the secondary B+ tree in Depth-first order
		print_sec_rec(secondaryRoot);
	}
	
	public void print_sec_rec(LibraryNode node){

		if ( node.type != LibraryNodeType.Leaf ){
			System.out.println("<index>");
			for(int i = 0;i<((LibraryNodeSecondaryIndex)node).keyCount();i++){
				System.out.println(((LibraryNodeSecondaryIndex)node).yearAtIndex(i)+"|"+((LibraryNodeSecondaryIndex)node).keyAtIndex(i));			
			}
			System.out.println("</index>");

			for(int i=0;i<((LibraryNodeSecondaryIndex)node).keyCount()+1;i++){
				print_sec_rec(((LibraryNodeSecondaryIndex)node).getChildrenAt(i));
			}
		}
		else{
			System.out.println("<data>");
			for(int i =0 ;i< ((LibraryNodeLeaf)node).bookCount();i++){
				CengBook book = ((LibraryNodeLeaf)node).bookAtIndex(i);
				System.out.println("<record>"+book.fullName()+"</record>");								
			}
			System.out.println("</data>");
		}
	}
	// Extra functions if needed
}
