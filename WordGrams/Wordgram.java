/*Brian Sisco
 *CS 445
 *HW 4: Wordgrams
 *Asks for a list of words and a phrase and then finds the words that are anagrams of the phrase.
 */
 
 import java.util.*;
 import java.io.*;
 
 public class Wordgram
 {
 	private static ArrayList<String> solutions = new ArrayList();
 	private static StringTable words;
 	private static StringTable prefixes;
 	
 	public static void main(String args[])
 	{
 		Scanner in = new Scanner(System.in);
 		System.out.print("Welcome to the Wordgram program.\nEnter Words File: ");
 		String input = in.next();
 		File f = new File(input);
 		if(!f.exists())
 			System.out.println("Specified file does not exist.");
 		else
 		{
 			try
 			{
 				FileReader fr = new FileReader(f);
	 			Scanner fScanner = new Scanner(fr);
	 			int line = 0;
	 			
	 			//Counts how many words there are.
	 			while(fScanner.hasNextLine())
	 			{
	 				String word = fScanner.nextLine();
	 				if(!word.trim().isEmpty())
	 					line++;
	 			}
	 			//System.out.println("There are " + line + " words in the file.");
	 			//Creates the hash table for the words
	 			words = new StringTable(line);
	 			fScanner.close();
	 			fScanner = new Scanner(new FileReader(f));
	 			int numPrefixes = 0;
	 			//adds the words to the hash table
	 			while(fScanner.hasNextLine())
	 			{
	 				String word = fScanner.nextLine();
	 				if(!word.trim().isEmpty())
	 				{
	 					words.add(word);
	 					numPrefixes += word.length();
	 				}
	 			}
	 			//creates a hash table for the prefixes
	 			prefixes = new StringTable(numPrefixes);
	 			fScanner.close();
	 			fScanner = new Scanner(new FileReader(f));
	 			//adds each prefix for each word
	 			while(fScanner.hasNextLine())
	 			{
	 				String word = fScanner.nextLine();
	 				word = word.trim();
	 				for(int i = 1; i < word.length() + 1; i++)
	 					prefixes.add(word.substring(0, i));
	 			}
	 			fScanner.close();
	 			
	 		}
 			catch(FileNotFoundException e)
 			{
 				//This shouldn't occur since before entering the try it is checked whether the file exists.
 				System.out.println("Error reading file.");
 			}
	 		
	 		do //keeps asking until they give a phrase that doesn't give any results.
	 		{
	 			//asks for the scrambled word
	 			System.out.print("Enter Wordgram, . to quit: ");
	 			input = in.next();
	 			
	 			permutations("", input);
	 			
	 			//prints out the solutions in brackets
	 			System.out.print("[");
	 			for(int i = 0; i < solutions.size() - 1; i++)
	 				System.out.print(solutions.get(i) + ", ");
	 			if(solutions.size() > 0)
	 				System.out.print(solutions.get(solutions.size() - 1));
	 			System.out.println("]");
	 			solutions.clear();
	 		}while(!input.equals("."));	
 		}
 	}
 	
 	//adds the right permutations to the ArrayList solutions
 	private static void permutations(String pre, String s)
 	{
 		if(s.length() == 0) //if all the letters are in the prefix, it might be a solution
 		{
 			if(words.has(pre)) //check if it is in the list of words
 			{
 				for(int i = 0; i < solutions.size(); i++) //check if this solution was already found somehow
 					if(solutions.get(i).equals(pre))
 						return;
 				solutions.add(pre); //add that solution!
 			}
 			return;
 		}
 		for(int i = 0; i < s.length(); i++)
 		{
 			//adds a character to the prefix
 			char temp = s.charAt(i);
 			pre = pre + temp;
 			//removes the character from the string
 			s = s.substring(0, i) + s.substring(i + 1);
 			//only continues with this prefix if it is in the hash table of prefixes
 			if(prefixes.has(pre))	
 				permutations(pre, s);
 			//removes the last character from the prefix string
 			pre = pre.substring(0, pre.length() - 1);
 			//puts the character back to where it was
 			s = s.substring(0, i) + temp + s.substring(i);
 		}
 		
 	}
 }
 		
 		