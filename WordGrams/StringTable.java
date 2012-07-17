/*Brian Sisco
 *CS 445
 *HW 4: Wordgrams
 *A custom implementation of a hash table for this project.
 *This particular implementation does not allow for resizing, though it can hold an "infinite" amount through collisions
 *In other words, this hash table is like a fixed amount of bottemless buckets.
 */
 
 import java.util.*;
 import java.math.*;
 
 public class StringTable
 {
 	private ArrayList[] keys;
 	
 	//this constructor makes the hash table be the next prime number after twice the number of items that will be in it.
 	//so there shouldn't be much collision, but if there is that's fine since it is an array of arraylists.
 	public StringTable(int s)
 	{
 		//finds the next prime after the input
 		Integer i = new Integer(s);
 		BigInteger b = new BigInteger(i.toString());
 		b = b.nextProbablePrime();
 		//compares the prime to the max int value.
 		BigInteger max = new BigInteger((new Integer(Integer.MAX_VALUE)).toString());
 		//makes the hash table's size be either the prime or the max int
 		int size = 0;
 		if(b.compareTo(max) < 0)
 			size = b.intValue();
 		else
 			size = Integer.MAX_VALUE;
 		keys = new ArrayList[size];
 	}
 	
 	//adds a string to the hash table
 	public void add(String s)
 	{
 		int h = hash(s);
 		//initializes the arraylist if it is empty/still null
 		if(keys[h] == null)
 			keys[h] = new ArrayList<String>();
 		keys[hash(s)].add(s);
 	}
 	
 	//hashes a string using Horner's Rule
 	private int hash(String s)
 	{
 		int hash = 0;
 		int g = 31;
 		int M = keys.length;
 		for(int i = 0; i < s.length(); i++)
 		{
 			int code = s.charAt(i);
 			hash = (hash * g + code) % M;
 		}
 		return hash;
 	}
 	
 	//returns true if the given string is in the hash table
 	public boolean has(String s)
 	{
 		int h = hash(s); //hash the string
 		if(keys[h] != null) //check if there are any strings at that hash
 			for(int i = 0; i < keys[h].size(); i++) //go through each one there
 				if(keys[h].get(i).equals(s))		//see if they are equal to the sought after one
 					return true;
 		return false;
 	}
 	
 	//prints out every string in the table, for testing purposes usually.
 	public String toString()
 	{
 		for(int i = 0; i < keys.length; i++)
 			if(keys[i] != null)
 				for(int j = 0; j < keys[i].size(); j++)
 					System.out.println(keys[i].get(j));
 		return null;
 	}
 }