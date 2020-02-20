import java.util.ArrayList;

public class LibraryNodePrimaryIndex extends LibraryNode
{
	private ArrayList<Integer> keys;
	private ArrayList<LibraryNode> children;	
	
	public LibraryNodePrimaryIndex(LibraryNode parent) 
	{
		super(parent);
		keys = new ArrayList<Integer>();
		children = new ArrayList<LibraryNode>();
		this.type = LibraryNodeType.Internal;
	}
	
	public LibraryNodePrimaryIndex(LibraryNode parent, ArrayList<Integer> keys, ArrayList<LibraryNode> children) 
	{
		super(parent);
		this.keys = keys;
		this.children = children;
		this.type = LibraryNodeType.Internal;
	}
	
	// GUI Methods - Do not modify
	public ArrayList<LibraryNode> getAllChildren()
	{
		return this.children;
	}
	
	public LibraryNode getChildrenAt(Integer index) {
		
		return this.children.get(index);
	}
	
	public Integer keyCount()
	{
		return this.keys.size();
	}
	public Integer keyAtIndex(Integer index)
	{
		if(index >= this.keyCount() || index < 0)
		{
			return -1;
		}
		else
		{
			return this.keys.get(index);			
		}
	}
	
	// Extra functions if needed
	public ArrayList<Integer> getKeys()
	{
		return this.keys;
	}

}
