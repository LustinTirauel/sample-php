<?php
	
class bibParser{
	public $bibtexID;
	public $entry;
	public $infostr;
	
	
	public function getEntry ($bibtexID, $bibtexField){ //that function gets a specific field (e.g., author) of a specific bibtext entry (e.g., wearables2021chi)
		
		
		if (file_exists("publications.bib"))
			$file = file ("publications.bib");
		
		else
			$file = file ("../publications.bib");
		
		$filestr = implode ("",$file);
		$splitbibtexentry = preg_split("/@/",$filestr);
		$searchstr = $bibtexID; 
		$m_array = preg_grep( "/$searchstr.*/", $splitbibtexentry);
		$keys = array_keys($m_array); //$m_array comes in this format [0] => "info1", [1] => "info2". So, even if there is only one element in that array, the number of the element changes, with array_keys function, we create another array which is like the Array([0] => "key of $m_array")
		$information = $m_array[$keys[0]]; //this is the specific bibtext entry which includes the ID we are looking for
		$result = $this -> bibtexInfo($information, $bibtexField);
		
		if ($bibtexField == "BibTeX"){ //this gives us the full bibtex entry for copying the citation
			$strbibtex = preg_split("/pubtype.*/",$information);
			$strbibtexhey = preg_split("/,(?! )/",$strbibtex [0]); // create a string array by splitting it after commas that do not have spaces (so leaving e.g., author string as it is)
			//print_r($strbibtex);
			
			
			for ($i=0; $i<sizeof($strbibtexhey)-1; $i++){//split above resuled in a array length one more than it should be, so we cut one here
				$result .= $strbibtexhey[$i];
				
				if ($i >= sizeof($strbibtexhey)-2) //we add only a line before the last entry
				$result .= ""; 
				
				else 
				$result .= ",";
			}
			
			$result = "@" . $result . "\n}";
			
		}
		
		return $result;

	}
	
	public function bibtexInfo($entry, $infostr){ //getting a specific info from a bibtex entry, this is usually used in the getEntry function and not much for separate use
	
		if ($infostr == "type"){ //usually the type of the entry is written in the beginning of the bibtext entry, here we try to get it. 
			$strposbracket1 = strpos($entry, '{'); //This variable gives the order number of the first character before '{' of the bibtex field e.g., inproceedings{abcd}, this finds the order number of "s", the latest letter of inproceedings
			$information = substr($entry, 0, $strposbracket1); // this returns the string inside ofthe brackets. It gets the [0] element of $keys, and returns the string starting

		}	
		
		else if ($infostr == "id"){ //usually the type of the entry is written in the beginning of the bibtext entry, here we try to get it. 
			$strposbracket1 = strpos($entry, '{') + 1; //This variable gives the order number of the first character after the first {"
			$strposbracket2 = strpos($entry, ',') - $strposbracket1 ; //This variable gives the order number of the first character before the first "," miinus the strposbracket11 to give the total number of characters after strposbracket1
			$information = substr($entry, $strposbracket1, $strposbracket2); // 
		}

		else{	
			$strbibtex = preg_split("/,(?! )/",$entry); // create a string array by splitting it after commas that do not have spaces (so leaving e.g., author string as it is)
			//print_r($strbibtex);
			$searchstr = $infostr; // the information we are looking for in the selected bibtex entry
			$m_array = preg_grep( "/\b$searchstr\b =/", $strbibtex); // this returns an array that has the matching searchstr among all other array elements e.g., if we put "id" as the searchstr $m_array will include that specific information
			$keys = array_keys($m_array); //$m_array comes in this format [0] => "info1", [1] => "info2". So, even if there is only one element in that array, the number of the element changes, with array_keys function, we create another array which is like the Array([0] => "key of $m_array")
			$information = ""; //define information as a blank string
			if (count ($keys) > 0){//go and grab information if information exists in the bibtex entry
				$strposbracket1 = strpos($strbibtex[$keys[0]], '{') + 1; //keys[0] includes the information we need. This variable gives the order number of the first character inside the brackets of the bibtex field e.g., author = {abcd}, this finds the order number of "a"
				$strposbracket2 = strpos($strbibtex[$keys[0]], '}') - $strposbracket1; //keys[0] includes the infornmation we need. This variable gives the length of the string in {} starting from the first needed character e.g., author = {abcd}, this finds the length of "abcd"
				$information = substr($strbibtex[$keys[0]],$strposbracket1, $strposbracket2); // this returns the string inside ofthe brackets. It gets the [0] element of $keys, and returns the string starting		
				}
			}
		
		return $information;
		
	}
	
	public function getContent ($maxNumber, $pubtype, $pagetype, $project, $category, $dateprioritized){ //this function creates repeated entries for different pages. it orders entries in descending chronology. MaxNumber defines the maximum number of entries that will be placed. Pubtype defines the specific types of publications (option: paper, extended_abstract, game), especially for grouping in the main page. Page type defines the type of page in which the content is fethched (option: main, project, publication). $Project is for project pages and fetches the content according to the project they belong. $category is for publication list and adds the publications in a speccific category, e.g., peer-reviewed conference proceedings, $dateprioritized will sort the content according to date in an descending order if enabled
		
		if (file_exists(__DIR__ . "/publications.bib"))
			$file = file (__DIR__ . "/publications.bib");
		
		else
			$file = file (__DIR__ . "/../publications.bib");
		
		$filestr = implode ("",$file);
		$splitbibtexentry = preg_split("/@/",$filestr);
		$contentArray = array(); //this is the array we made from the featured entries
		$contentArrayIDs = array(); //we will populate the IDs of the features arrays here
		$contentArrayDates = array(); //we will populate dates here for making a chronological ordering
		$contentArrayCombined = array(); //this is for combining above arrays
		$z = 0; //this increases as we add new elements to featured array			
				
		for ($i=1; $i<sizeof($splitbibtexentry); $i++)
		{
			
			$id = $this -> bibtexInfo($splitbibtexentry[$i], "id");
			$featuredConfirmation = $this -> getEntry($id, "featured");
			$pubtypecheck = $this -> getEntry($id, "pubtype");
			$projectcheck = $this -> getEntry($id, "project");
			$categorycheck = $this -> getEntry($id, "category");
			$year = $this -> getEntry($id, "year");
			
			
			if ($pubtype == "paper"){
				if ($pubtypecheck == "Full Paper" || $pubtypecheck == "Pictorial" || $pubtypecheck == "Short Paper" || $pubtypecheck == "Book")
					$pubtypecheck = "yes";
			}
			
			if ($pubtype == "extended_abstract"){
				if ($pubtypecheck == "Extended Abstract")
					$pubtypecheck = "yes";
			}
			
			if ($pubtype == "game"){
				if ($pubtypecheck == "Game")
					$pubtypecheck = "yes";
			}
			
			
			if ($pagetype == "main" && $featuredConfirmation == "yes" && $pubtypecheck == "yes"){ //if making the main page, the featured parts are "yes," and the publication type is correct, than add them to the array
				$contentArrayIDs[$z] = $id;
				$contentArrayDates[$z] = $year;
				$contentArrayCombined = array_combine($contentArrayIDs,$contentArrayDates); //this creates and array like thie array (towards2021chi => 2021, exploring2022chiplay => 2022)
				$z++;
			}
			
			else if ($pagetype == "project" && $projectcheck == $project){ //if making the project page, if the project name is matching, add them to the array
				$contentArrayIDs[$z] = $id;
				$contentArrayDates[$z] = $year;
				$contentArrayCombined = array_combine($contentArrayIDs,$contentArrayDates); //this creates and array like thie array (towards2021chi => 2021, exploring2022chiplay => 2022)
				$z++;
			}
			
			else if ($pagetype == "publication" && $pubtypecheck != "Game"){ //if making the publication page, add all items to array
				$contentArrayIDs[$z] = $id;
				$contentArrayDates[$z] = $year;
				$contentArrayCombined = array_combine($contentArrayIDs,$contentArrayDates); //this creates and array like this array (towards2021chi => 2021, exploring2022chiplay => 2022)
				$maxnumber = sizeof($contentArrayCombined);
				$z++;
			}
			
			else if ($pagetype == "publicationslist" && $categorycheck == $category){ //if making the publication list, add all items to array
				$contentArrayIDs[$z] = $id;
				$contentArrayDates[$z] = $year;
				$contentArrayCombined = array_combine($contentArrayIDs,$contentArrayDates); //this creates and array like this array (towards2021chi => 2021, exploring2022chiplay => 2022)
				$maxnumber = sizeof($contentArrayCombined);
				$z++;
			}
			
		}

		if ($dateprioritized)
		arsort ($contentArrayCombined);	 //here we sort the array by values which are the years if dateprioritized is enabled
		
		$contentArrayCombined = array_keys($contentArrayCombined); //here we only get the keys which are the ids
		
		if ($maxNumber > sizeof($contentArrayCombined))	//this is for limiting to the max number of items. If total number of items are smaller than the max number than the max number is equal to max number of items
			$maxNumber = sizeof($contentArrayCombined);
			
		for ($i=0; $i<$maxNumber; $i++) //add max number of items to the content array
			$contentArray [$i] = $contentArrayCombined[$i];
			
		return $contentArray;

	}
	
	public function getProjectEntries ($projectName){ //that function gets a specific field (e.g., author) of a specific bibtext entry (e.g., wearables2021chi)
		
		$file = file ("publications.bib");
		$filestr = implode ("",$file);
		$projectArray = array();
		$projectArrayIDs = array(); 
		$projectArrayDates = array();
		$projectArrayCombined = array();
		$z = 0;
		
		$splitbibtexentry = preg_split("/@/",$filestr);
				
			
		for ($i=1; $i<sizeof($splitbibtexentry); $i++)
		{
			
			$id = $this -> bibtexInfo($splitbibtexentry[$i], "id");
			$featuredConfirmation = $this -> getEntry($id, "featured");
			$year = $this -> getEntry($id, "year");
			
			if ($featuredConfirmation == "yes"){ //if the featured parts are "yes," than add them to the array
				$featuredArrayIDs[$z] = $id;
				$featuredArrayDates[$z] = $year;
				$featuredArrayCombined = array_combine($featuredArrayIDs,$featuredArrayDates);
				$z++;
			}
			
		}
			
//		$searchstr = $bibtexID;
//		$m_array = preg_grep( "/$searchstr.*/", $splitbibtexentry);
//		$keys = array_keys($m_array); //$m_array comes in this format [0] => "info1", [1] => "info2". So, even if there is only one element in that array, the number of the element changes, with array_keys function, we create another array which is like the Array([0] => "key of $m_array")
//		$information = $m_array[$keys[0]]; //this is the specific bibtext entry which includes the ID we are looking for
//		$result = bibtexInfo($information, $bibtexField);
		
		arsort ($featuredArrayCombined);	
		$featuredArrayCombined = array_keys($featuredArrayCombined);
		
		if ($maxNumber > sizeof($featuredArrayCombined))	
			$maxNumber = sizeof($featuredArrayCombined);
			
		for ($i=0; $i<$maxNumber; $i++)
			$featuredArray [$i] = $featuredArrayCombined[$i];
			
			
		return $featuredArray;

	}
	
	public function getAuthorName($bibtexID, $authornumber, $style){ //get specific author names in the reference style wanted
		
		$authornames = $this -> getEntry($bibtexID, "author"); //first get the author section of the related bibtex entry
		$authorstring = preg_split("/ and /",$authornames); // create a string array of author names
		
		if ($this -> getEntry($bibtexID, "shortauthors") != null){
			$shortauthornames = $this -> getEntry($bibtexID, "shortauthors"); //get short author names for the main page
			$shortauthorstring = preg_split("/ and /",$shortauthornames);
		}
		
		if ($style == "SHORT"){ //if the selected style is SHORT get them from the separate entry called shortauthors. This was specifically created for using short author names in the main page when needed. 
				
			if ($authornumber<sizeof($shortauthorstring)){
				$authornamearray = preg_split("/, /",$shortauthorstring[$authornumber]); //divide the author name by the comma
				$authorfirstname = $authornamearray[1]; //make the second entry of the array the firstname of the author
				$authorlastname = $authornamearray[0]; //make the first entry of the array the last name of the author
				$authorname = $authorfirstname . " " . $authorlastname; //create the authornamestring
			}
			
			else
				$authorname = "";
		}
			
			
		
		
		if ($style == "SIGCHI"){ //if the selected style is SIGCHI simply add reverse the authorfirst name and the second name{
				
			if ($authornumber<sizeof($authorstring)){
				$authornamearray = preg_split("/, /",$authorstring[$authornumber]); //divide the author name by the comma
				$authorfirstname = $authornamearray[1]; //make the second entry of the array the firstname of the author
				$authorlastname = $authornamearray[0]; //make the first entry of the array the last name of the author
				$authorname = $authorfirstname . " " . $authorlastname; //create the authornamestring
			}
			
			else
				$authorname = "";
		
		}
		
		if ($style == "APA"){ //this is a bit more complicated since we need to get the initials of authors' given names
				$authornamearray = preg_split("/, /",$authorstring[$authornumber]);
				//print_r($authornamearray);
				$authorfirstname = $authornamearray[1];
				$authorlastname = $authornamearray[0];
				//print_r($authorfirstname);
				$authorsgivennames = preg_split ("/ (?!')/u",$authorfirstname, -1, PREG_SPLIT_NO_EMPTY); //create an array of author's all given names. Do not consider it another name if there is a nick name written in with '' symbols. For example: Oğuz 'Oz' Buruk
				$count = count ($authorsgivennames); //the array length of the author's given names
				$authorsgivennamesinitials = ""; //variable for initials
			
					for ($i = 0; $i < $count; $i++){ //add the first letters of each given name with a for loop
						$authorsgivennamesarray = preg_split ("//u",$authorsgivennames[$i], -1, PREG_SPLIT_NO_EMPTY); //create an array including all letters of the given name [i]. "u" is for UTF-8 encoding
						$authorsgivennamesinitials .= " "; //start creating the string with a space
						$authorsgivennamesinitials .= $authorsgivennamesarray[0]; //add the first letter of the of the given name
						$authorsgivennamesinitials .= "."; //add a dot after the initial
						}

				$authorfirstnamearray = preg_split ("//u",$authorfirstname, -1, PREG_SPLIT_NO_EMPTY);
				$authorname = $authorlastname . "," . $authorsgivennamesinitials;
			}
		
		if ($style == "MLA") {
				$authornamearray = preg_split("/, /",$authorstring[$authornumber]);
				//print_r($authornamearray);
				$authorfirstname = $authornamearray[1];
				$authorlastname = $authornamearray[0];
				//print_r($authorfirstname);
				$authorsgivennames = preg_split ("/ (?!')/u",$authorfirstname, -1, PREG_SPLIT_NO_EMPTY); //create an array of author's all given names. Do not consider it another name if there is a nick name written in with '' symbols. For example: Oğuz 'Oz' Buruk
				$count = count ($authorsgivennames); //the array length of the author's given names
				$authorsgivennamesinitials = ""; //variable for initials
			
			

					for ($i = 1; $i < $count; $i++){ //add the first letters of each given name with a for loop. Since MLA uses the first full name, this starts if there is a second name.
	
						$authorsgivennamesarray = preg_split ("//u",$authorsgivennames[$i], -1, PREG_SPLIT_NO_EMPTY); //create an array including all letters of the given name [i]. "u" is for UTF-8 encoding
						$authorsgivennamesinitials .= " "; //start creating the string with a space
						$authorsgivennamesinitials .= $authorsgivennamesarray[0]; //add the first letter of the of the given name
						$authorsgivennamesinitials .= "."; //add a dot after the initial
						}
					

					$authorfirstnamearray = preg_split ("//u",$authorfirstname, -1, PREG_SPLIT_NO_EMPTY);
					$authorname = $authorlastname . ", " . $authorsgivennames[0] . $authorsgivennamesinitials;
			}
		
		if ($style == "Harvard") { //same as APA
				$authornamearray = preg_split("/, /",$authorstring[$authornumber]);
				//print_r($authornamearray);
				$authorfirstname = $authornamearray[1];
				$authorlastname = $authornamearray[0];
				//print_r($authorfirstname);
				$authorsgivennames = preg_split ("/ (?!')/u",$authorfirstname, -1, PREG_SPLIT_NO_EMPTY); //create an array of author's all given names. Do not consider it another name if there is a nick name written in with '' symbols. For example: Oğuz 'Oz' Buruk
				$count = count ($authorsgivennames); //the array length of the author's given names
				$authorsgivennamesinitials = ""; //variable for initials
			
					for ($i = 0; $i < $count; $i++){ //add the first letters of each given name with a for loop
						$authorsgivennamesarray = preg_split ("//u",$authorsgivennames[$i], -1, PREG_SPLIT_NO_EMPTY); //create an array including all letters of the given name [i]. "u" is for UTF-8 encoding
						$authorsgivennamesinitials .= " "; //start creating the string with a space
						$authorsgivennamesinitials .= $authorsgivennamesarray[0]; //add the first letter of the of the given name
						$authorsgivennamesinitials .= "."; //add a dot after the initial
						}

				$authorfirstnamearray = preg_split ("//u",$authorfirstname, -1, PREG_SPLIT_NO_EMPTY);
				$authorname = $authorlastname . "," . $authorsgivennamesinitials;
			}
		
		if ($style == "Vancouver") {
				$authornamearray = preg_split("/, /",$authorstring[$authornumber]);
				//print_r($authornamearray);
				$authorfirstname = $authornamearray[1];
				$authorlastname = $authornamearray[0];
				//print_r($authorfirstname);
				$authorsgivennames = preg_split ("/ (?!')/u",$authorfirstname, -1, PREG_SPLIT_NO_EMPTY); //create an array of author's all given names. Do not consider it another name if there is a nick name written in with '' symbols. For example: Oğuz 'Oz' Buruk
				$count = count ($authorsgivennames); //the array length of the author's given names
				$authorsgivennamesinitials = ""; //variable for initials
			
					for ($i = 0; $i < $count; $i++){ //add the first letters of each given name with a for loop
						$authorsgivennamesarray = preg_split ("//u",$authorsgivennames[$i], -1, PREG_SPLIT_NO_EMPTY); //create an array including all letters of the given name [i]. "u" is for UTF-8 encoding
						$authorsgivennamesinitials .= ""; //start creating the string with a space
						$authorsgivennamesinitials .= $authorsgivennamesarray[0]; //add the first letter of the of the given name
						}

				$authorfirstnamearray = preg_split ("//u",$authorfirstname, -1, PREG_SPLIT_NO_EMPTY);
				$authorname = $authorlastname . " " . $authorsgivennamesinitials;
			}
		
		if ($style == "Chicago") { //same as MLA
				$authornamearray = preg_split("/, /",$authorstring[$authornumber]);
				//print_r($authornamearray);
				$authorfirstname = $authornamearray[1];
				$authorlastname = $authornamearray[0];
				//print_r($authorfirstname);
				$authorsgivennames = preg_split ("/ (?!')/u",$authorfirstname, -1, PREG_SPLIT_NO_EMPTY); //create an array of author's all given names. Do not consider it another name if there is a nick name written in with '' symbols. For example: Oğuz 'Oz' Buruk
				$count = count ($authorsgivennames); //the array length of the author's given names
				$authorsgivennamesinitials = ""; //variable for initials

					for ($i = 1; $i < $count; $i++){ //add the first letters of each given name with a for loop. Since MLA uses the first full name, this starts if there is a second name.
	
						$authorsgivennamesarray = preg_split ("//u",$authorsgivennames[$i], -1, PREG_SPLIT_NO_EMPTY); //create an array including all letters of the given name [i]. "u" is for UTF-8 encoding
						$authorsgivennamesinitials .= " "; //start creating the string with a space
						$authorsgivennamesinitials .= $authorsgivennamesarray[0]; //add the first letter of the of the given name
						$authorsgivennamesinitials .= "."; //add a dot after the initial
						}

					$authorfirstnamearray = preg_split ("//u",$authorfirstname, -1, PREG_SPLIT_NO_EMPTY);
					$authorname = $authorlastname . ", " . $authorsgivennames[0] . $authorsgivennamesinitials;
			}
		
		if ($style == "ISO960") { //same as APA
				$authornamearray = preg_split("/, /",$authorstring[$authornumber]);
				//print_r($authornamearray);
				$authorfirstname = $authornamearray[1];
				$authorlastname = mb_strtoupper ($authornamearray[0], "UTF-8"); //turn the surname to upper case
				
				//print_r($authorfirstname);
				$authorsgivennames = preg_split ("/ (?!')/u",$authorfirstname, -1, PREG_SPLIT_NO_EMPTY); //create an array of author's all given names. Do not consider it another name if there is a nick name written in with '' symbols. For example: Oğuz 'Oz' Buruk
				$count = count ($authorsgivennames); //the array length of the author's given names
				$authorsgivennamesinitials = ""; //variable for initials
			
					for ($i = 0; $i < $count; $i++){ //add the first letters of each given name with a for loop
						$authorsgivennamesarray = preg_split ("//u",$authorsgivennames[$i], -1, PREG_SPLIT_NO_EMPTY); //create an array including all letters of the given name [i]. "u" is for UTF-8 encoding
						$authorsgivennamesinitials .= " "; //start creating the string with a space
						$authorsgivennamesinitials .= $authorsgivennamesarray[0]; //add the first letter of the of the given name
						$authorsgivennamesinitials .= "."; //add a dot after the initial
						}

				$authorfirstnamearray = preg_split ("//u",$authorfirstname, -1, PREG_SPLIT_NO_EMPTY);
				$authorname = $authorlastname . "," . $authorsgivennamesinitials;
			}
		
		return $authorname;
	}
	
	public function getCoAuthorNames ($bibtexID, $mainAuthor, $style){
		$authornames = $this -> getEntry($bibtexID, "author"); //first get the author section of the related bibtex entry
		$fullauthorarray = preg_split("/ and /", $authornames); // create a string array of author names
		$authorarray = array();
		$mainauthorkey = array_search($mainAuthor, $fullauthorarray); //find the key of the main author

		
		for ($i=0; $i<sizeof($fullauthorarray); $i++){ //make a new array without the main author in the given style
			if ($i != $mainauthorkey){
				$authorstyledname = $this -> getAuthorName($bibtexID, $i, $style);
				array_push ($authorarray, $authorstyledname);
			}
		}
		$coauthors = "";
		
		for ($i=0; $i<sizeof($authorarray); $i++){ //list author names 
				$coauthors .= $authorarray [$i];
				if ($i < sizeof($authorarray)-2)
					$coauthors .= ", ";
				
				if ($i == sizeof($authorarray)-2) //add and to the author before the last author
				
				if (sizeof($authorarray)-2 == 0)
					$coauthors .= "";
			} 
		
		return $coauthors;		
	}
	
	public function getCitation($bibtexID, $style, $bolden, $boldname){ //APA, MLA, SIGCHI, Harvard Vancouver, ISO9600
			$authornames = $this -> getEntry($bibtexID, "author"); //get author names from bibtex
			$authornamesarray = preg_split("/ and /",$authornames); // create an array with each author name
			$authorcount = sizeof ($authornamesarray);
			$authorfield = "";	
		
			if($this -> getEntry($bibtexID, "type") != null)
				$type = $this -> getEntry($bibtexID, "type");
		
			if ($this -> getEntry($bibtexID, "journal") != null)
				$booktitle = $this -> getEntry($bibtexID, "journal"); //sometimes the title can be in the journal field, so if this exist take this one
			
			if ($this -> getEntry($bibtexID, "booktitle") != null)
				$booktitle = $this -> getEntry($bibtexID, "booktitle");
			
			else if ($this -> getEntry($bibtexID, "journal") == null && $this -> getEntry($bibtexID, "booktitle") == null)
				$booktitle = "";
			
			if ($this -> getEntry($bibtexID, "volume") != null)
				$volume = $this -> getEntry($bibtexID, "volume");
			else
				$volume = ""; //if volume info does not exist make it blank
		
			if ($this -> getEntry($bibtexID, "pages") != null)
				$pages = $this -> getEntry($bibtexID, "pages"); //if pages are given in a range, directly take it
			else
				$pages = "";
		
			if ($this -> getEntry($bibtexID, "numpages") != null)
			 	$numpages = $this -> getEntry($bibtexID, "numpages"); //if numpages are given in a range, directly take it
			else
				$numpages = "";
		
			if ($this -> getEntry($bibtexID, "month") != null)
				$month = $this -> getEntry($bibtexID, "month");
			else
				$month = "";
		
			if ($this -> getEntry($bibtexID, "issue") != null)
				$issue = $this -> getEntry($bibtexID, "issue");
			else
				$issue = "";
		
			if ($this -> getEntry($bibtexID, "issue_date") != null)
				$issuedate = $this -> getEntry($bibtexID, "issue_date");
			else
				$issuedate = "";
		
			if ($this -> getEntry($bibtexID, "articleno") != null)
				$articleno = $this -> getEntry($bibtexID, "articleno");
			else
				$articleno = "";
		
			if ($this -> getEntry($bibtexID, "series") != null)
				$series = $this -> getEntry($bibtexID, "series");
			else
				$series = "";
		
			if ($this -> getEntry($bibtexID, "year") != null)
				$datepublished = $this -> getEntry($bibtexID, "year");
			else
				$datepublished = ""; //if info does not exist make it blank
		
			if ($this -> getEntry($bibtexID, "title") != null)
				$title = $this -> getEntry($bibtexID, "title");
			else
				$title = ""; //if info does not exist make it blank
		
			if ($this -> getEntry($bibtexID, "doi") != null)
				$doi = $this -> getEntry($bibtexID, "doi");
			else
				$doi = ""; //if info does not exist make it blank
		
			if ($this -> getEntry($bibtexID, "url") != null)
				$url = $this -> getEntry($bibtexID, "url");
			else
				$url = ""; //if info does not exist make it blank
			
			if ($this -> getEntry($bibtexID, "address") != null)
				$publisherlocation = $this -> getEntry($bibtexID, "address");
			else
				$publisherlocation = ""; //if info does not exist make it blank
		
			if ($this -> getEntry($bibtexID, "publisher") != null)
				$publisher = $this -> getEntry($bibtexID, "publisher");
			else
				$publisher = ""; //if info does not exist make it blank
		
			if ($this -> getEntry($bibtexID, "number") != null)
				$number = $this -> getEntry($bibtexID, "number");
			else
				$number = ""; //if info does not exist make it blank
		
		
		if ($style == "SIGCHI"){
			
			for ($i=0; $i<$authorcount; $i++){
				$authorname = $this -> getAuthorName($bibtexID, $i, $style);
				
				//if bolden is active add all author names with the specific author bolded
				if ($bolden == 1)
				{
					if (strpos($authorname, $boldname) !== false && $authorname != "Seda Suman Buruk")
						$authorfield .= "<b>" . $authorname . "</b>";
					else
						$authorfield .= $authorname;	
						
				}
				
					if ($bolden == 0)
				{
					$authorfield .= $authorname;
						
				}
				
				
			
				//add "and" before the last author
				if ($i < $authorcount-2){
				$authorfield .= ", ";
				}
				if ($i == $authorcount-2){
				$authorfield .= " and ";
				}
				
			}
			
				$citation = $authorfield . ". ";
			
				if ($datepublished != "")
					$citation .= $datepublished . ". ";
				else
					$citation .= "n.d.";
			
				if ($title != "")
					$citation .= $title . ". ";
			
			
				if ($type == "inproceedings")
					$citation .= "In ";
			
				if ($booktitle != "")
					$citation .= "<i>" . $booktitle . "</i>";
				
				if ($series != "") {
					
				if ($type == "inproceedings")
					$citation .= "<i> (" . $series . ").</i> "; }
				
				if ($volume != "")
					$citation .= " " . $volume;
			
				if ($number != "")
					$citation .= " " . $number;
			
				else
					$citation .= "";
			
				
				if ($type == "inproceedings" || $type == "book"){
					if ($publisher != "")
					$citation .= $publisher;
			
					if ($publisherlocation != "")
					$citation .= ", " . $publisherlocation;	
				}
				
				
				
				if ($issue != "")
					$citation .= $issue;
			
				if ($articleno != "")
					$citation .= ", Article " . $articleno;
			
				if ($issuedate != "")
					$citation .= " (" . $issuedate . ")";
			
//				if ($issue != "" || $articleno != "" || $issuedate != "")
//				$citation .= ", ";
				
				if ($pages != "")
					$citation .= ", " . $pages;

				if ($numpages != ""){
					if ($type == "inproceedings")
							$citation .= " 1-" . $numpages;

					if ($type == "inproceedings" || $type == "book")
							$citation .= $numpages . " pages";
				}	
			
				$citation .= ".";
			
				if ($doi != "")
					$citation .= " <a link href=\"https://doi.org/" . $doi . "\" target=\"_blank\">" . "https://doi.org/" . $doi . "</a>"; //add doi with link
			
				if ($doi == "" && $url != "")
					$citation .= " <a link href=\"" . $url  . "\" target=\"_blank\">". $url . "</a>";
			}
		
		if ($style == "APA"){
			for ($i=0; $i<$authorcount; $i++){
				$authorname = $this -> getAuthorName($bibtexID, $i, $style);
				
				
				if ($bolden == 1)
				{
					if (strpos($authorname, $boldname) !== false)
						$authorfield .= "<b>" . $authorname . "</b>";
					else
						$authorfield .= $authorname;	
						
				}
				
					if ($bolden == 0)
				{
					$authorfield .= $authorname;
						
				}
					
				//add "&" before the last author
				if ($i < $authorcount-1){
				$authorfield .= ",";
				}
				if ($i == $authorcount-2){
				$authorfield .= " & ";
				}
				$authorfield .= " ";
			}
			
			$citation = $authorfield;
			
				if ($datepublished != "")
					$citation .= " (" . $datepublished . "). ";
				else
					$citation .= " (n.d). ";
			
				if ($title != "")
					$citation .= $title . ". ";
			
			
				if ($type == "inproceedings")
					$citation .= "In ";
			
				if ($booktitle != "")
					$citation .= "<i>" . $booktitle . "</i> ";
			
				if ($series != ""){

					if ($type == "inproceedings")
						$citation .= "<i>(" . $series . ").</i> ";
				}
				else
					$citation .= "";
				
				if ($volume != ""){
					$citation .= "<i>". $volume . "</i>";
			
				if ($issue != "")
					$citation .= "(". $issue . ")";
				
				if ($number != "")
					$citation .= "(". $number . ")";
			
					$citation .= ", ";
				}
			
				if ($articleno != "")
					$citation .= " Article " . $articleno;
			
				if ($issuedate != "")
					$citation .= " (" . $issuedate . ")";
				
				if ($issue != "" || $articleno != "" || $issuedate != "")
				$citation .= ", ";
			
				if ($type == "inproceedings")
					$citation .= " pp.";
			
				if ($pages != "")
					$citation .= " " . $pages . ". ";
			
				if ($numpages != "")
					$citation .= " 1-" . $numpages . ". ";
				
			
				
				if ($type == "inproceedings"){
								
					if ($publisherlocation != "")
					$citation .= $publisherlocation . ", ";	
					
					if ($publisher != "")
					$citation .= $publisher . ", ";

				}
						
				if ($doi != "")
					$citation .= " <a link href=\"https://doi.org/" . $doi . "\" target=\"_blank\">" . "https://doi.org/" . $doi . "</a>"; //add doi with link
			
				if ($doi == "" && $url != "")
					$citation .= " <a link href=\"" . $url  . "\" target=\"_blank\">". $url . "</a>";

		}
		
		if ($style == "MLA"){
			
			if ($authorcount == 1){
			$authorname = $this -> getAuthorName($bibtexID, 0, $style);
				
				if ($bolden == 1)
				{
					if (strpos($authorname, $boldname) !== false)
						$authorfield .= "<b>" . $authorname . "</b>";
					else
						$authorfield .= $authorname;	
						
				}
				
					if ($bolden == 0)
				{
					$authorfield .= $authorname . ". ";
						
				}
				
			}
			
			if ($authorcount == 2){
			$authorname1 = $this -> getAuthorName($bibtexID, 0, $style);
			$authorname2 = $this -> getAuthorName($bibtexID, 1, "SIGCHI");

				if ($bolden == 1)
				{
					if (strpos($authorname1, $boldname) !== false)
						$authorname1 = "<b>" . $authorname1 . "</b>";
					if (strpos($authorname2, $boldname) !== false)
						$authorname2 = "<b>" . $authorname2 . "</b>";
				}
				
				
				$authorfield =$authorname1 . " and " . $authorname2 . ".";
				
			
			}
			
			
			if ($authorcount > 2){
				$authorname = $this -> getAuthorName($bibtexID, 0, $style);
				
				if ($bolden == 1)
				{
					if (strpos($authorname, $boldname) !== false)
						$authorname = "<b>" . $authorname . "</b>";
				}
				
				$authorfield = $authorname . " et al.";
			}
			
			$citation = $authorfield;
			
			if ($title != "")
					$citation .= " \"". $title . "\". ";
			
			if ($type == "inproceedings"){
					$citation .= "In ";
			
			if ($booktitle != "")
				$citation .= "<i>" . $booktitle . "</i>";

			if ($series != "")
					$citation .= "<i> (" . $series . ")</i>";
				
				$citation .= ", ";
			}
			
			
			if ($type == "article")
				$citation .= "<i>" . $booktitle . ", </i>";
		
					
			if ($volume != "")
				$citation .= "vol. ". $volume ." ";

			if ($issue != "")
				$citation .= "(". $issue . "), ";
			
			if ($number != "")
				$citation .= "no. ". $number . ", ";

			if ($issuedate != "")
				$citation .= " (" . $issuedate . "), ";

				
			if ($type == "inproceedings"){

				if ($publisherlocation != "")
				$citation .= $publisherlocation . ", ";	

				if ($publisher != "")
				$citation .= $publisher . ", ";

			}
			
			if ($datepublished != "")
				$citation .= $datepublished . ", ";
			else
				$citation .= " (n.d). ";
			
			if ($articleno != "")
				$citation .= " Article " . $articleno .", ";

			
			if ($pages != "")
				$citation .= "pp. " . $pages . ". ";
			
			if ($numpages != "")
					$citation .= "pp. 1-" . $numpages . ". ";

			else
				$citation .= "";
						
			if ($doi != "")
				$citation .= " <a link href=\"https://doi.org/" . $doi . "\" target=\"_blank\">" . "https://doi.org/" . $doi . "</a>"; //add doi with link

			if ($doi == "" && $url != "")
				$citation .= " <a link href=\"" . $url  . "\" target=\"_blank\">". $url . "</a>";

		}
		
		if ($style == "Harvard"){
			for ($i=0; $i<$authorcount; $i++){
				$authorname = $this -> getAuthorName($bibtexID, $i, $style);
				
				
				if ($bolden == 1)
				{
					if (strpos($authorname, $boldname) !== false)
						$authorfield .= "<b>" . $authorname . "</b>";
					else
						$authorfield .= $authorname;	
						
				}
				
					if ($bolden == 0)
				{
					$authorfield .= $authorname;
						
				}
					
				//add "&" before the last author
				if ($i < $authorcount-1){
				$authorfield .= ",";
				}
				if ($i == $authorcount-2){
				$authorfield .= " and ";
				}
				$authorfield .= " ";
			}
			
			$citation = $authorfield;
			
				if ($datepublished != "")
					$citation .= " (" . $datepublished . "). ";
				else
					$citation .= " (n.d). ";
			
				if ($title != "")
					$citation .= "‘" . $title . "’, ";
			
			
				if ($type == "inproceedings")
					$citation .= "In ";
			
				if ($booktitle != "")
					$citation .= "<i>" . $booktitle . "</i> ";
			
				if ($series != ""){

					if ($type == "inproceedings")
						$citation .= "<i>(" . $series . "),</i> ";
				}
				else
					$citation .= "";
				
				if ($volume != ""){
					$citation .= "<i>". $volume . "</i>";
			
				if ($issue != "")
					$citation .= "(". $issue . ")";
			
					$citation .= ", ";
				}
			
				if ($articleno != "")
					$citation .= " Article " . $articleno;
			
				if ($issuedate != "")
					$citation .= " (" . $issuedate . ")";
				
				if ($issue != "" || $articleno != "" || $issuedate != "")
				$citation .= ", ";
			
				if ($pages != "")
					$citation .= "p. " . $pages . ". ";
			
				if ($numpages != "")
					$citation .= "p. 1-" . $numpages . ". ";
				
				if ($publisherlocation != "")
					$citation .= $publisherlocation . ": ";	
					
				if ($publisher != "")
					$citation .= $publisher . ", ";

				if ($doi != "")
					$citation .= " <a link href=\"https://doi.org/" . $doi . "\" target=\"_blank\">" . "https://doi.org/" . $doi . "</a>"; //add doi with link
			
				if ($doi == "" && $url != "")
					$citation .= " <a link href=\"" . $url  . "\" target=\"_blank\">". $url . "</a>";

		}
		
		if ($style == "Vancouver"){
			for ($i=0; $i<$authorcount && $i<6; $i++){
				$authorname = $this -> getAuthorName($bibtexID, $i, $style);
				
				
				if ($bolden == 1)
				{
					if (strpos($authorname, $boldname) !== false)
						$authorfield .= "<b>" . $authorname . "</b>";
					else
						$authorfield .= $authorname;	
						
				}
				
					if ($bolden == 0)
				{
					$authorfield .= $authorname;
						
				}
					
				//add "&" before the last author
				if ($i < $authorcount-1){
				$authorfield .= ", ";
				}
				
				
			}
			
			if ($authorcount > 6)
				$authorfield .= " et al";
			
			$citation = $authorfield . ". ";
			
			if ($title != "")
				$citation .= $title . ". ";

			if ($type == "inproceedings")
				$citation .= "In: ";

			if ($booktitle != "")
				$citation .= "<i>" . $booktitle . "</i>";
			
			if ($series != ""){

				if ($type == "inproceedings")
					$citation .= " " . "<i>(" . $series . ")</i>";
			}
			
			$citation .= ". ";
			
			if ($this -> getEntry($bibtexID, "type") == "book" || $type == "inproceedings"){		
				if ($publisherlocation != "")
				$citation .= $publisherlocation . ": ";	

				if ($publisher != "")
					$citation .= $publisher . "; ";
			}
			
			if ($datepublished != "")
				$citation .= " " . $datepublished;
			else
				$citation .= " (n.d)";

			if ($type == "inproceedings" || $type == "book")
				$citation .= ";";
			
			if ($type == "inproceedings")
				$citation .= ".";

			if ($volume != "")
				$citation .= $volume;

			if ($issue != "")
				$citation .= "(". $issue . ")";
			
			if ($number != "")
				$citation .= "(". $number . ")";

			if ($articleno != "")
				$citation .= " Article " . $articleno;

			if ($issuedate != "")
				$citation .= " (" . $issuedate . ")";
			
			if ($pages != "")
				$citation .= ": p. " . $pages . ". ";

			if ($numpages != "")
				$citation .= ": p. 1-" . $numpages . ". ";
		
			if ($doi != "")
				$citation .= " <a link href=\"https://doi.org/" . $doi . "\" target=\"_blank\">" . "https://doi.org/" . $doi . "</a>"; //add doi with link

			if ($doi == "" && $url != "")
				$citation .= " <a link href=\"" . $url  . "\" target=\"_blank\">". $url . "</a>";

		}
		
		if ($style == "Chicago"){
			for ($i=0; $i<$authorcount; $i++){
				$authorname = $this -> getAuthorName($bibtexID, $i, $style);
				if ($i > 0){
					$authorname = $this -> getAuthorName($bibtexID, $i, "SIGCHI");
				}
				
				//if bolden is active add all author names with the specific author bolded
				if ($bolden == 1)
				{
					if (strpos($authorname, $boldname) !== false)
						$authorfield .= "<b>" . $authorname . "</b>";
					else
						$authorfield .= $authorname;	
						
				}
				
					if ($bolden == 0)
				{
					$authorfield .= $authorname;
						
				}
				
			
			
				//add "and" before the last author
				if ($i < $authorcount-2){
				$authorfield .= ", ";
				}
				if ($i == $authorcount-2){
				$authorfield .= ", and ";
				}
				
			}
			
			$citation = $authorfield;
			
			if ($title != "")
					$citation .= " \"". $title . ".\" ";
			
			if ($type == "inproceedings")
					$citation .= "In ";
			
			if ($booktitle != "")
				$citation .= "<i>" . $booktitle . "</i>";

			if ($series != ""){

				if ($type == "inproceedings")
					$citation .= "<i> (" . $series . ")</i>, ";
			}	
					
			if ($volume != ""){
				$citation .= " <i>". $volume . "</i>, ";
				
			if ($number != "")
				$citation .= "no. ". $number;

			if ($issue != "")
				$citation .= "(". $issue . ")";

				$citation .= ", ";
			}

	

				
			if ($type == "inproceedings"){

				if ($publisherlocation != "")
				$citation .= $publisherlocation . ", ";	

				if ($publisher != "")
				$citation .= $publisher . ", ";

			}
			
			if ($datepublished != "")
				$citation .= "(" . $datepublished . "): ";
			else
				$citation .= " (n.d). ";
			
			if ($articleno != "")
			$citation .= " Article " . $articleno . ",";
			
			if ($pages != "")
				$citation .= $pages . ". ";
			
			if ($numpages != "")
					$citation .= "1-" . $numpages . ". ";

			else
				$citation .= "";
						
			if ($doi != "")
				$citation .= " <a link href=\"https://doi.org/" . $doi . "\" target=\"_blank\">" . "https://doi.org/" . $doi . "</a>"; //add doi with link

			if ($doi == "" && $url != "")
				$citation .= " <a link href=\"" . $url  . "\" target=\"_blank\">". $url . "</a>";

		}
		
		if ($style == "ISO960"){
			for ($i=0; $i<$authorcount; $i++){
				$authorname = $this -> getAuthorName($bibtexID, $i, $style);
				
				
				if ($bolden == 1)
				{
					if (strpos($authorname, $boldname) !== false)
						$authorfield .= "<b>" . $authorname . "</b>";
					else
						$authorfield .= $authorname;	
						
				}
				
					if ($bolden == 0)
				{
					$authorfield .= $authorname;
						
				}
					
				//add "&" before the last author
				if ($i < $authorcount-1)
				$authorfield .= ";";

				$authorfield .= " ";
			}
			
			$citation = $authorfield;
			
			if ($title != "")
					$citation .= $title . ". ";
			
		
				if ($type == "inproceedings")
					$citation .= "In: ";
			
				if ($booktitle != "")
					$citation .= "<i>" . $booktitle . "</i> ";
			
				if ($series != ""){

					if ($type == "inproceedings")
						$citation .= "<i>(" . $series . ").</i> ";
				}
				else
					$citation .= "";
				
						

				if ($publisherlocation != "")
				$citation .= $publisherlocation . ": ";	

				if ($publisher != "")
				$citation .= $publisher . ", ";


			
				if ($datepublished != "")
					$citation .= " " . $datepublished . ". ";
				else
					$citation .= " (n.d). ";
			
				if ($volume != ""){
					$citation .= "<b>". $volume . "</b>";
			
				if ($issue != "")
					$citation .= "(". $issue . ")";
				
				if ($number != "")
					$citation .= "(". $number . ")";
			
					$citation .= ", ";
				}
			
				if ($articleno != "")
					$citation .= " Article " . $articleno;
			
				if ($issuedate != "")
					$citation .= " (" . $issuedate . ")";
				
				if ($issue != "" || $articleno != "" || $issuedate != "")
				$citation .= ", ";
			
				if ($type == "inproceedings")
					$citation .= " pp.";
			
				if ($pages != "")
					$citation .= " " . $pages . ". ";
			
				if ($numpages != "")
					$citation .= " 1-" . $numpages . ". ";
					
				if ($doi != "")
					$citation .= " <a link href=\"https://doi.org/" . $doi . "\" target=\"_blank\">" . "https://doi.org/" . $doi . "</a>"; //add doi with link
			
				if ($doi == "" && $url != "")
					$citation .= " <a link href=\"" . $url  . "\" target=\"_blank\">". $url . "</a>";

		}
		
		return $citation;
		
	} 
	
	public function produceCitationBox($bibtexID){
		
		$SIGCHI = $this -> getCitation($bibtexID,"SIGCHI",0,"buruk");
		$APA = $this -> getCitation($bibtexID,"APA",0,"buruk");
		$MLA = $this -> getCitation($bibtexID,"MLA",0,"buruk");
		$Harvard = $this -> getCitation($bibtexID,"Harvard",0,"buruk");
		$Vancouver = $this -> getCitation($bibtexID,"Vancouver",0,"buruk");
		$Chicago = $this -> getCitation($bibtexID,"Chicago",0,"buruk");
		$ISO960 = $this -> getCitation($bibtexID,"ISO960",0,"buruk");
		$BibTeX = $this -> getEntry($bibtexID,"BibTeX");
		
		$citationBoxArray = array("SIGCHI" => $SIGCHI, "APA" => $APA, "MLA" => $MLA, "Harvard" => $Harvard,"Vancouver" => $Vancouver,"Chicago" => $Chicago, "ISO960" => $ISO960);
		//print_r(array_values($citationBoxArray)[0]);
		
		$citationBox = "<div style=\"display: none;\" id=\"";
		$citationBox.= $bibtexID . "Cite\">
					
											<div align=\"center\" role=\"group\">
												<a id = \"BibTeXbutton\" class =\"btn btn-default\" onclick=\"copy('BibTeX', 'BibTeXbutton')\">
												<b>BibTeX</b> <z class=\"far fa-clipboard\"></z> <br>
												</a>
												<p hidden id=\"BibTeX\">" . $BibTeX . "</p><br>
											</div>
											<div>";
		
			for ($i=0; $i<sizeof($citationBoxArray); $i++){
				$citationBox .= "<div class=\"citeContainer\">

												  <section class=\"citesection\">
												  <a id = \"". array_keys($citationBoxArray)[$i] . "button\" class =\"btn btn-copy\" onclick=\"copySelected('". array_keys($citationBoxArray)[$i] ."', '" . array_keys($citationBoxArray)[$i] . "button', event)\">
													<citep><b>". array_keys($citationBoxArray)[$i] ."</b> <br> <z class=\"far fa-clipboard\"></z></citep>
												  </a> 
												  </section>
												  <section class=\"citep\">
													<p onClick=\"selectText(id)\" id=\"". array_keys($citationBoxArray)[$i] . "\">" . array_values($citationBoxArray)[$i] . "</p>
												  </section>
												</div>";				
			}
		
               $citationBox .=	"
			   			</div>	
				";
		
		return $citationBox;
		
	}
	
	public function publicationCards($bibtexID){
		
		$author1 = $this -> getAuthorName($bibtexID, 0, "SIGCHI");
		
		if ($this -> getAuthorName($bibtexID, 1, "SIGCHI") != null)
			$author2 = $this -> getAuthorName($bibtexID, 1, "SIGCHI");
		else $author2 = "";
		
		if ($this -> getAuthorName($bibtexID, 2, "SIGCHI") != null)
			$author3 = $this -> getAuthorName($bibtexID, 2, "SIGCHI");
		else $author3 = "";
		
		if ($this -> getAuthorName($bibtexID, 3, "SIGCHI") != null)
			$author4 = ", et al.";
		else $author4 = "";
	
		if ($this -> getEntry($bibtexID, "shortauthors") != null){ //if there is a shortauthor entry in Bibtex, take it instead of the full ones for the main page. 
			$author1 = $this -> getAuthorName($bibtexID, 0, "SHORT");

			if ($this -> getAuthorName($bibtexID, 1, "SHORT") != null)
				$author2 = $this -> getAuthorName($bibtexID, 1, "SHORT");
			else $author2 = "";
			
			if ($this -> getAuthorName($bibtexID, 2, "SHORT") != null)
				$author3 = $this -> getAuthorName($bibtexID, 2, "SHORT");
			else $author3 = "";
			
			if ($this -> getAuthorName($bibtexID, 3, "SHORT") != null)
				$author4 = " et al.";
			else $author4 = "";
		}
		
		if ($this -> getEntry($bibtexID, "title") != null)
				$title = $this -> getEntry($bibtexID, "title");
		
		if ($this -> getEntry($bibtexID, "shortvenue") != null)
				$shortvenue = $this -> getEntry($bibtexID, "shortvenue");
		else $shortvenue = "";
		
		if ($this -> getEntry($bibtexID, "shorttitle") != null)
				$title = $this -> getEntry($bibtexID, "shorttitle");
		
		if ($this -> getEntry($bibtexID, "h5index") != null)
				$h5 = $this -> getEntry($bibtexID, "h5index");
		else $h5 = "";
		
		if ($this -> getEntry($bibtexID, "impact") != null)
				$impact = $this -> getEntry($bibtexID, "impact");
		else $impact = "";
		
		if ($this -> getEntry($bibtexID, "doi") != null)
				$doi = $this -> getEntry($bibtexID, "doi");
		else $doi = "";
		
		if ($this -> getEntry($bibtexID, "url") != null)
				$url = $this -> getEntry($bibtexID, "url");
		else $url = "";
		
		
		if ($this -> getEntry($bibtexID, "acceptrate") != null)
				$acceptrate = $this -> getEntry($bibtexID, "acceptrate");
		else $acceptrate = "";
		
		if ($this -> getEntry($bibtexID, "publisher") != null)
				$publisher = $this -> getEntry($bibtexID, "publisher");
		else
				$publisher = "";
		
		if ($publisher == "Association for Computing Machinery")
			$publisher = "ACM";
		
		if ($publisher == "Springer International Publishing")
			$publisher = "Springer";
		
		if ($publisher == "International Journal of Design")
			$publisher = "IJ Design";
		
		if ($this -> getEntry($bibtexID, "abstract") != null)
				$abstract = $this -> getEntry($bibtexID, "abstract");
		
		if ($this -> getEntry($bibtexID, "slidesurl") != null)
				$slidesurl = $this -> getEntry($bibtexID, "slidesurl");
		else $slidesurl = "";
		
		if ($this -> getEntry($bibtexID, "videourl") != null)
				$videourl = $this -> getEntry($bibtexID, "videourl");
		else $videourl = "";
		
		if ($this -> getEntry($bibtexID, "pubtype") != null)
				$pubtype = $this -> getEntry($bibtexID, "pubtype");
		
		if ($this -> getEntry($bibtexID, "pubcontext") != null)
				$pubcontext = $this -> getEntry($bibtexID, "pubcontext");
		else $pubcontext = "";
		
		if ($this -> getEntry($bibtexID, "platform") != null)
				$platform = $this -> getEntry($bibtexID, "platform");
		else $platform = "";
		
		if ($pubcontext == "Student Game Design Competition Finalist")
			$pubcontext = "SGDC Finalist";
		
		if ($this -> getEntry($bibtexID, "series") != null)
				$series = $this -> getEntry($bibtexID, "series");
		else $series = "";
		
		if ($this -> getEntry($bibtexID, "award") != null)
				$award = $this -> getEntry($bibtexID, "award");
		else $award = "";
		
		if ($this -> getEntry($bibtexID, "project") != null)
				$project = $this -> getEntry($bibtexID, "project");
		
		if ($this -> getEntry($bibtexID, "note") != null)
				$note = $this -> getEntry($bibtexID, "note");
		else $note = "";
		
		if ($this -> getEntry($bibtexID, "buttons") != null){ //we define required buttons in bibtex file and here we add all of them to the summary
			$buttons = $this -> getEntry($bibtexID, "buttons");
			$buttonsArray = explode ("," , $buttons);
			$buttonsArrayText = array();
			$buttonsArrayLink = array();
			
	
		for ($i=0; $i < sizeof($buttonsArray); $i++){ //creating the button array if there are extra buttons that needs to be added
			$buttonsArrayText[$i] = explode ("|" , $buttonsArray[$i])[0];
			$buttonsArrayLink[$i] = explode ("|" , $buttonsArray[$i])[1];
			
			
			if (substr($buttonsArrayText[$i], 2, 3) == "Int"){// this is a special case for IX Mag
				$buttonsArrayText[$i] = "IX Mag";
			}
			}
			
		}
		
		else {
			$buttons = "";
			$buttonsArray = "";		
		}
		
		$SIGCHI = $this -> getCitation($bibtexID,"SIGCHI",0,"buruk");
		$APA = $this -> getCitation($bibtexID,"APA",0,"buruk");
		$MLA = $this -> getCitation($bibtexID,"MLA",0,"buruk");
		$Harvard = $this -> getCitation($bibtexID,"Harvard",0,"buruk");
		$Vancouver = $this -> getCitation($bibtexID,"Vancouver",0,"buruk");
		$Chicago = $this -> getCitation($bibtexID,"Chicago",0,"buruk");
		$ISO960 = $this -> getCitation($bibtexID,"ISO960",0,"buruk");
		$BibTeX = $this -> getEntry($bibtexID,"BibTeX");
		
		
		$publicationcardcode = "
			<div class=\"col-lg-3 col-md-3 col-sm-6 col-xs-6 thumbnail-padding\">
				<div class=\"thumbnail";
		
		if ($award == "honorablemention" || $award == "Crystal Pixel Best Game Design and Most Innovative Game Nominee")
		$publicationcardcode .= " honor";
		
		if ($award == "sgdcac2" || $award == "Best Poster")
		$publicationcardcode .= " award";
		
		if ($pubtype != "Game"){
		$publicationcardcode .="\">
					<a href=\"pages/". $project . ".php#" . $bibtexID . "\"><img src=\"images/" . $bibtexID . "_thumb.jpg\" alt=\"Thumbnail Image 1\" width=\"500\" height=\"250\" class=\"img-responsive cardclip\"></a>
					<h8><a href=\"pages/" . $project . ".php#" . $bibtexID . "\">" . $title . " </a><p class=\"text-center margin-10-sides margin-8-top authortext\">- " . $author1 . ", " . $author2 . ", " . $author3 . $author4 . " -</p></h8>
					<div class=\"container-conferenceInfo lg-mt-0 lg-mt-0\">
					  <p class=\"grayed text-center margin-10-sides\">";
			
			if ($series != null)
				$publicationcardcode .= $series;
			
			if ($shortvenue != null)
				$publicationcardcode .= $shortvenue;
			
			
		}
		
		else if ($pubtype == "Game"){
			$publicationcardcode .="\">
					<a href=\"pages/". $project . ".php#" . $bibtexID . "\"><img src=\"images/" . $bibtexID . "_thumb.jpg\" alt=\"Thumbnail Image 1\" width=\"500\" height=\"250\" class=\"img-responsive cardclip\"></a>
					<h9><a href=\"pages/" . $project . ".php#" . $bibtexID . "\">" . $title . " </a><p class=\"text-center margin-10-sides margin-8-top authortext\">- " . $author1 . ", " . $author2 . ", " . $author3 . $author4 . " -</p></h9>
					<div class=\"container-conferenceInfo lg-mt-0 lg-mt-0\">
					  <p class=\"grayed text-center margin-10-sides\">" . $series;
		}
		
		
		
		if ($pubtype == "Full Paper" || $pubtype == "Short Paper" || $pubtype == "Pictorial" || $pubtype == "Book"){ //full paper and extended abstract layout is slighlty different, this is why we have this condition
		
			
			$publicationcardcode .= " " . $pubtype;
			
		if ($h5 != null || $impact != null)
			$publicationcardcode .=	" [";
		
		if($h5 != null)
		$publicationcardcode .=	"h5-index: " . $h5;
		
		
		if($impact != null){
			if ($h5 != null)
			$publicationcardcode .=	", ";
			
		$publicationcardcode .=	"impact factor: " . $impact;
		}
		
		if ($h5 != null || $impact != null)
		$publicationcardcode .=	"]";
		
	
		if ($acceptrate != null)
			$publicationcardcode .="</p>
					  <p class=\"grayed text-center margin-10-sides\">[acceptance rate: " . $acceptrate . "%]&nbsp;&nbsp;</p>";
			
		if ($acceptrate == null)
			$publicationcardcode .="</p>
					  <p class=\"grayed text-center margin-10-sides\">&nbsp;&nbsp;</p>";
		}
		
		
		
		if ($pubtype == "Extended Abstract"){ //full paper and extended abstract layout is slighlty different, this is why we have this condition
		if ($pubcontext != "")
			$publicationcardcode .=	" (" . $pubcontext . ")";
		
		if ($h5 != null || $impact != null)
			$publicationcardcode .=	"<p class=\"grayed text-center margin-10-sides\">[";
		
		if($h5 != null)
		$publicationcardcode .=	"h5-index: " . $h5;
		
		
		if($acceptrate != null){
			if ($h5 != null)
			$publicationcardcode .=	", ";
			
		$publicationcardcode .=	"acceptance rate: " . $acceptrate . "%";
		}
		
		if ($h5 != null || $acceptrate != null)
		$publicationcardcode .=	"]";
			
		}
		
		
		if ($pubtype == "Game"){
			$publicationcardcode .=	$platform . " Game";
			$publicationcardcode .=	"</p>
					  <p class=\"grayed text-center margin-10-bottom margin-10-sides\">" . $note . "</p>";
			
		if ($bibtexID != "stc2014")
			$publicationcardcode .=	"
					  <p class=\"grayed text-center margin-10-sides\">&nbsp;&nbsp;</p>";
		}
		
	
		if ($pubtype != "Game"){
		
		if ($doi == null && $url != null)
			$doi = $url;
		
		else
			$doi = "https://doi.org/" . $doi;
		
		$publicationcardcode .=	"
					  <p class=\"grayed text-center margin-10-sides\">&nbsp;&nbsp;</p>
					</div>

					<div align=\"center\" role=\"group\">
					  <a data-fancybox data-options='{\"src\": \"#" . $bibtexID . "Abs\", \"touch\": false}' href=\"javascript:;\" class=\"btn btn-default\">Abstract</a>";
					  
					  if (file_exists("Resources/Papers/". $bibtexID . ".pdf"))
					  		$publicationcardcode .= "\n<a target=\"_blank\" href=\"Resources/Papers/". $bibtexID . ".pdf\" class=\"btn btn-default\">PDF</a>\n";
			
					  if (!file_exists("Resources/Papers/". $bibtexID . ".pdf"))
					  		$publicationcardcode .= "\n<a target=\"_blank\" href=\"Resources/Papers/". $bibtexID . ".pdf\" class=\"btn btn-default disabled\">PDF</a>\n";
			
					  $publicationcardcode .= "<a data-fancybox data-options='{\"src\": \"#". $bibtexID . "Cite\", \"touch\": false}' href=\"javascript:;\" class=\"btn btn-default\">Cite</a>
					  <a target=\"_blank\" href=\"" . $doi . "\" class=\"btn btn-default\">" . $publisher . "</a>
					  ";
		}
		
		if ($buttonsArray != "" && $pubtype == "Game"){ //if game don't have predefined buttons and add only custom ones  
			for ($i=0; $i<sizeof($buttonsArray); $i++){
														
				if (substr($buttonsArrayLink[$i],0,9) != " disabled"){
				$publicationcardcode .= "<a target=\"_blank\" href=\"" . $buttonsArrayLink[$i] . "\" class=\"btn btn-default\">" . $buttonsArrayText[$i] . "</a>\n";
				}

				if (substr($buttonsArrayLink[$i],0,9) == " disabled"){//make a disabled button if the link is disabled
				$publicationcardcode .= "<a target=\"_blank\" href=\"" . $buttonsArrayLink[$i] . "\" class=\"btn btn-default disabled\">" . $buttonsArrayText[$i] . "</a>\n";
				}
			
				
			//print("i=" . $i . "-link=" . $buttonsArrayLink[$i] . " ");
			}
		}
		
		
			if ($buttonsArray != "" && $pubtype != "Game"){ //if there are extra buttons defined to a publication such as video or slides, this code adds it. 
				for ($i=0; $i<1; $i++){
					$publicationcardcode .= "<a target=\"_blank\" href=\"" . $buttonsArrayLink[$i] . "\" class=\"btn btn-default\">" . $buttonsArrayText[$i] . "</a>\n";
				}
			}
		
			

			$publicationcardcode .="
			</div>";

			if ($pubtype != "Game"){
			$publicationcardcode .= $this -> produceCitationBox($bibtexID);
			$publicationcardcode .=	"
					</div>

					<div style=\"display: none;\" id=\"" . $bibtexID . "Abs\">
						<div class=\"absContainer\">
							<div class=\"absContainer\">
							  <section class=\"col-lg-12\">
								<h3>" . $title . "</h3>
								</p> </section>
							</div>

							<div class=\"absContainer\">
							  <section class=\"col-lg-12\">
								<h4>Abstract</h4></p> </section>
							</div>


							<div class=\"absContainer\">
							  <section onClick=\"selectText(this.id)\" id=\"elementID\" class=\"col-lg-12\">" . $abstract . "</p> </section>
							</div>

						</div>	
					</div> 
				</div>
			</div> 
		"; }
		
		if ($pubtype == "Game"){
			$publicationcardcode .=	"
				</div>
			</div> 
		"; }
		
		return $publicationcardcode;
		
		}
	
	public function publicationSummary($bibtexID){
		
		if ($this -> getEntry($bibtexID, "title") != null)
				$title = $this -> getEntry($bibtexID, "title");
		
		if ($this -> getEntry($bibtexID, "h5index") != null)
				$h5 = $this -> getEntry($bibtexID, "h5index");
		else $h5 = "";
		
		if ($this -> getEntry($bibtexID, "impact") != null)
				$impact = $this -> getEntry($bibtexID, "impact");
		else $impact = "";
		
		if ($this -> getEntry($bibtexID, "award") != null)
				$award = $this -> getEntry($bibtexID, "award");
		else $award = "";
		
		if ($this -> getEntry($bibtexID, "doi") != null)
				$doi = $this -> getEntry($bibtexID, "doi");
		else
			$doi = "";
		
		if ($this -> getEntry($bibtexID, "url") != null)
				$url = $this -> getEntry($bibtexID, "url");
		else
			$url = "";
		
		if ($this -> getEntry($bibtexID, "acceptrate") != null)
				$acceptrate = $this -> getEntry($bibtexID, "acceptrate");
		else $acceptrate = "";
		
		if ($this -> getEntry($bibtexID, "abstract") != null)
				$abstract = $this -> getEntry($bibtexID, "abstract");
		
		if ($this -> getEntry($bibtexID, "slidesurl") != null)
				$slidesurl = $this -> getEntry($bibtexID, "slidesurl");
		else $slidesurl = "";
		
		if ($this -> getEntry($bibtexID, "videourl") != null)
				$videourl = $this -> getEntry($bibtexID, "videourl");
		else $videourl = "";
		
		if ($this -> getEntry($bibtexID, "pubtype") != null)
				$pubtype = $this -> getEntry($bibtexID, "pubtype");
		
		if ($this -> getEntry($bibtexID, "pubcontext") != null)
				$pubcontext = $this -> getEntry($bibtexID, "pubcontext");
		else
			$pubcontext ="";
		
		if ($this -> getEntry($bibtexID, "series") != null)
				$series = $this -> getEntry($bibtexID, "series");
		
		if ($this -> getEntry($bibtexID, "booktitle") != null)
				$booktitle = $this -> getEntry($bibtexID, "booktitle");
		else
			$booktitle ="";
		
		if ($this -> getEntry($bibtexID, "journal") != null)
				$journal = $this -> getEntry($bibtexID, "journal");
		else
			$journal ="";
		
		if ($this -> getEntry($bibtexID, "year") != null)
				$year = $this -> getEntry($bibtexID, "year");
		
		if ($this -> getEntry($bibtexID, "role") != null)
				$role = $this -> getEntry($bibtexID, "role");
		
		if ($this -> getEntry($bibtexID, "publisher") != null)
				$publisher = $this -> getEntry($bibtexID, "publisher");
		else
				$publisher = "";
		
		if ($publisher == "Association for Computing Machinery")
			$publisher = "ACM";
		
		if ($publisher == "Springer International Publishing}")
			$publisher = "Springer";
		
		if ($this -> getEntry($bibtexID, "carousel") != null)
				$carousel = $this -> getEntry($bibtexID, "carousel");
		else
				$carousel = "";
		
		if ($this -> getEntry($bibtexID, "note") != null)
				$note = $this -> getEntry($bibtexID, "note");
		else
				$note = "";
		
		if ($this -> getEntry($bibtexID, "buttons") != null){ //we define required buttons in bibtex file and here we add all of them to the summary
			$buttons = $this -> getEntry($bibtexID, "buttons");
			$buttonsArray = explode ("," , $buttons);
			$buttonsArrayText = array();
			$buttonsArrayLink = array();
			
	
		for ($i=0; $i < sizeof($buttonsArray); $i++){
			$buttonsArrayText[$i] = explode ("|" , $buttonsArray[$i])[0];
			$buttonsArrayLink[$i] = explode ("|" , $buttonsArray[$i])[1];
			}
		}
		
		else {
			$buttons = "";
			$buttonsArray = "";		
		}
		
		if ($this -> getEntry($bibtexID, "videocarousel") != null){ //we define different videos for the video carousel in bibtex file
			$vCarousel = $this -> getEntry($bibtexID, "videocarousel");
			$vCarouselArray = explode ("," , $vCarousel);
			$vCarouselArrayText = array();
			$vCarouselArrayLink = array();
			
	
		for ($i=0; $i < sizeof($vCarouselArray); $i++){
			$vCarouselArrayText[$i] = explode ("|" , $vCarouselArray[$i])[0];
			$vCarouselArrayLink[$i] = explode ("|" , $vCarouselArray[$i])[1];
			}
		}
		
		else {
			$vCarousel = "";
			$vCarouselArray = "";		
		}
				
		$coauthornames = $this -> getCoAuthorNames ($bibtexID,"Buruk, Oğuz 'Oz'","SIGCHI");
		
		
		//projectSummaryCode starts here
		$projectSummaryCode = "<div class=\"container projectPageContainer\" id=\"" . $bibtexID . "\">";
		
		if ($award != ""){
			$awardText = "";
			if ($award == "honorablemention" || $award == "Crystal Pixel Best Game Design and Most Innovative Game Nominee}"){
				$awardText = "awardBlack.png";
			}
			if ($award == "Student Game Design Competition, Audience Choice, 2nd Place" || $award == "Best Poster"){
				$awardText = "awardCupBlack.png";
			}
				
			
			if ($bibtexID == "extracting2018chi"){ //this code is specific to the entry that got the honorable mention so that the formatting looks good
				$projectSummaryCode .= "<div class=\"container projectSubtitleFrame\"><projectPageSubTitles class=\"col-lg-8 col-md-12 col-sm-12 col-xs-12\"> <div class= \"col-lg-1 col-md-1 col-sm-2 col-xs-2\"><img src=\"../images/" . $awardText ."\" width=\"40\" height=\"40\" class=\"img-responsive col-lg-10 col-md-8 col-sm-8 col-xs-8\" alt=\"awardIcon\"></div> <div class=\"col-lg-10 col-md-11 col-sm-10 col-xs-10\">". $title . "</div></projectPageSubTitles>";
			}
			
			else {
			$projectSummaryCode .= "<div class=\"container projectSubtitleFrame\"><projectPageSubTitles class=\"col-lg-6 col-md-12 col-sm-12 col-xs-12\"> <div class= \"col-lg-1 col-md-1 col-sm-2 col-xs-2\"><img src=\"../images/" . $awardText ."\" width=\"40\" height=\"40\" class=\"img-responsive col-lg-10 col-md-8 col-sm-8 col-xs-8\" alt=\"awardIcon\"></div> <div class=\"col-lg-11 col-md-11 col-sm-10 col-xs-10\">". $title . "</div></projectPageSubTitles>";
			}
			
		}
		
		else {
			$projectSummaryCode .= "<div class=\"container projectSubtitleFrame\">
			<projectPageSubTitles class=\"col-lg-6 col-md-6 col-sm-12 col-xs-12\">" . $title . "</projectPageSubTitles>";}
		
			$projectSummaryCode .= "</div>

									<div class=\"container projectPageFrames\">

									<div class=\"col-lg-6 projectPageAbs col-md-6 col-sm-12 col-xs-12 lg-pr-20 md-pr-20\">
										<p class=\"text-justify\"><strong>Abstract</strong></p>
										<p class=\"text-justify\">" . $abstract . "</p>";
									if ($note != "")
										$projectSummaryCode .= $note;				
										$projectSummaryCode .= "<p class=\"text-justify\">&nbsp;</p>
										<p class=\"text-left\"><strong>Role: </strong>" . $role. "</p>";
			
			if ($award != null)	
				$projectSummaryCode .= "<p class=\"text-left\"><strong>Award: </strong>" . $award. "</p>";
			
		
			$projectSummaryCode .= 		"<p class=\"text-left\"><strong>Type: </strong>" . $pubtype . " ";
			
										if($pubcontext != null)
											$projectSummaryCode .=	"(" . $pubcontext . ")</p>"; //extended abstracts might be in different contexts e.g., poster, demo, so we add it here
										else
											$projectSummaryCode .=	"</p>";
		
										if ($booktitle != null)
										$projectSummaryCode .= "<p class=\"text-left\"><strong>Conference: </strong>" . $booktitle . "</p>";
						
		
										if ($journal != null)
										$projectSummaryCode .= "<p class=\"text-left\"><strong>Journal: </strong>" . $journal . "</p>";
											
										if ($acceptrate != null || $h5 != null || $impact != null)
										$projectSummaryCode .= "<p class=\"text-left\"><strong>Stats: </strong>"; 
										
										if($acceptrate != null) //after that point we add the information (accept rate, h5, impact factor) based on the availability 
											$projectSummaryCode .=	"Acceptance rate: " . $acceptrate . "%";
										
										if($h5 != null){
											if($acceptrate != null)
											$projectSummaryCode .= ", ";
												
											$projectSummaryCode .=	"h5-index:" . $h5;
										}
		
		
										if($impact != null){
											if($acceptrate != null || $h5 != null)
											$projectSummaryCode .= ", ";
												
											$projectSummaryCode .=	"impact factor: " . $impact;
										}


										$projectSummaryCode .="
										</p>
										<p class=\"text-left\"><strong>Date: </strong>" . $year . "</p>
										<p class=\"text-left\"><strong>Co-Authors: </strong>". $coauthornames  . "</p>
										<br>
										</div>";
										
										
										if ($carousel > 0){ //add this one if there is a image carousel
											
											$projectSummaryCode .= "<div id=\"carousel" . $bibtexID  . "\" class=\"carousel slide col-lg-6 col-md-6 col-sm-12 col-xs-12\" data-ride=\"carousel\">\n<ol class=\"carousel-indicators\">\n<li data-target=\"#carousel" . $bibtexID . "\" data-slide-to=\"0\" class=\"active\"></li>\n";
												
											for ($i=1; $i<$carousel; $i++){
											$projectSummaryCode .= "<li data-target=\"#carousel1\"  data-slide-to=\"". $i . "\"></li>\n";
								
											}
											
											$projectSummaryCode .= "</ol>\n";
																			
											$projectSummaryCode .= "<div class=\"carousel-inner\" role=\"listbox\">\n";
											
											$projectSummaryCode .= "<div class=\"item active\"><a data-fancybox=\"" . $bibtexID . "_image\" href=\"../images/". $bibtexID . " (1).jpg\"><img src=\"../images/" . $bibtexID . " (1).jpg\" class=\"img-responsive\" alt=\"\"></a>\n</div>\n";
											
											for ($i=2; $i<=$carousel; $i++){
												$projectSummaryCode .= "<div class=\"item\"><a data-fancybox=\"" . $bibtexID . "_image\" href=\"../images/". $bibtexID . " (". $i . ").jpg\"><img src=\"../images/" . $bibtexID . " (". $i . ").jpg\" class=\"img-responsive\" alt=\"\"></a>\n</div>\n";
											}
											
											$projectSummaryCode .= "\n</div>\n<a class=\"left carousel-control\" href=\"#carousel" . $bibtexID  . "\" role=\"button\" data-slide=\"prev\"><span class=\"glyphicon glyphicon-chevron-left\" aria-hidden=\"true\"></span><span class=\"sr-only\">Previous</span></a><a class=\"right carousel-control\" href=\"#carousel" . $bibtexID  . "\" role=\"button\" data-slide=\"next\"><span class=\"glyphicon glyphicon-chevron-right\" aria-hidden=\"true\"></span><span class=\"sr-only\">Next</span></a></div>\n";
											
											
												
										}
		
										else if ($vCarousel != ""){
										$projectSummaryCode .="<div role=\"tabpanel\">
	
												  <div id=\"tabContent1\" class=\"tab-content\">
												  
												  <div role=\"tabpanel\" class=\"tab-pane fade in active\" id=\"home1\">
														<div class=\"embed-responsive col-lg-6 embed-responsive-16by9 col-md-6 col-sm-12 sm-pb-55 lg-pb-28per col-xs-12 xs-pb-55per\">
															<iframe width=\"560\" height=\"315\" src=\"" . $vCarouselArrayLink[0] . "\"  frameborder=\"0\" allowfullscreen></iframe>
														</div>
													</div>";
											for($i=1; $i<sizeof($vCarouselArray); $i++){
												$projectSummaryCode .= "<div role=\"tabpanel\" class=\"tab-pane fade\" id=\"pane" . $i . "\">
														<div class=\"embed-responsive col-lg-6 embed-responsive-16by9 col-md-6 col-sm-12 sm-pb-55 lg-pb-28per col-xs-12 xs-pb-55per\">
															<iframe width=\"560\" height=\"315\" src=\"" . $vCarouselArrayLink[$i] . "\"  frameborder=\"0\" allowfullscreen></iframe>
														</div>
													</div>";
											}

												$projectSummaryCode .= "<div class = \"container col-lg-6 visible-lg\"></div>  
													  <ul class=\"nav nav-tabs col-lg-6 col-md-6 col-sm-12 col-xs-12\" role=\"tablist\">
														<li role=\"presentation\" class=\"active\"><a href=\"#home1\" data-toggle=\"tab\" role=\"tab\" aria-controls=\"tab1\">". $vCarouselArrayText[0] ."</a></li>";
														
														for($i=1; $i < sizeof($vCarouselArray); $i++){
															$projectSummaryCode .= "<li role=\"presentation\"><a href=\"#pane" . $i . "\" data-toggle=\"tab\" role=\"tab\" aria-controls=\"tab". $i+1 ."\">" . $vCarouselArrayText[$i] . "</a></li>";
														}
														
												$projectSummaryCode .= "</ul>
													</div>
												</div>";
										}
		
										else if ($videourl != ""){									
											$projectSummaryCode .= "<div class=\"embed-responsive col-lg-6 embed-responsive-16by9 col-md-6 col-sm-12 sm-pb-55 lg-pb-28per col-xs-12\">
											<iframe width=\"560\" height=\"315\" src=\"" . $videourl .  "\"frameborder=\"0\" allowfullscreen></iframe>
											</div>"; 
										}
										
									  	else if ($carousel == "" && $vCarousel== ""){
										
											$projectSummaryCode .= "<a data-fancybox=\"" . $bibtexID . "_image\" href=\"../images/" . $bibtexID . ".jpg\"><img src=\"../images/";
											
											$image = $bibtexID. ".jpg";
											
											
											if(file_exists("../images/" . $bibtexID. "_small.jpg"))	
												$image =	$bibtexID. "_small.jpg";

											$projectSummaryCode .= $image . "\" class=\"img-responsive col-lg-6 col-md-6 col-sm-12 col-xs-12\" alt=\"\"></a>";
										}

										
		
										if ($doi == null && $url != null)
											$doi = $url;
		
										else
											$doi = "https://doi.org/" . $doi;
											
		
										$projectSummaryCode .= "<div class=\"projectButtons col-lg-6 col-md-6 md-mt-20 xl-mt-35- col-sm-12 sm-mt-10 col-xs-12 xs-mt-10\" role=\"group\">";
		
		
										if(file_exists("../Resources/Papers/" . $bibtexID . ".pdf")){				
											$projectSummaryCode .="
											<a target=\"_blank\" href=\"../Resources/Papers/" . $bibtexID . ".pdf\" class=\"btn btn-default\">PDF</a>\n";
										}

										if(!file_exists("../Resources/Papers/" . $bibtexID . ".pdf")){
											$projectSummaryCode .="
											<a target=\"_blank\" href=\"../Resources/Papers/" . $bibtexID . ".pdf\" class=\"btn btn-default disabled\">PDF</a>\n";
										}
		
										$projectSummaryCode .= "<a data-fancybox data-options='{\"src\": \"#" . $bibtexID . "Cite\", \"touch\": false}' href=\"javascript:;\" class=\"btn btn-default\">Cite</a>\n
											<a target=\"_blank\" href=\"" . $doi . "\" class=\"btn btn-default\">" . $publisher . "</a>\n";
										
										if ($buttonsArray != ""){
										for ($i=0; $i<sizeof($buttonsArray); $i++){
											
											$projectSummaryCode .= "<a target=\"_blank\" href=\"" . $buttonsArrayLink[$i] . "\" class=\"btn btn-default\">" . $buttonsArrayText[$i] . "</a>\n";
																		
										}
										}
											
											$projectSummaryCode .= "
										    </div>";
											
											$projectSummaryCode .= $this -> produceCitationBox($bibtexID);
											$projectSummaryCode .="
											</div>
											</div>
											</div>";
		
		
		
		return $projectSummaryCode;
		
		}
	
	public function publicationPage($bibtexID, $number){
		
		if ($this -> getEntry($bibtexID, "title") != null)
				$title = $this -> getEntry($bibtexID, "title");
		
		if ($this -> getEntry($bibtexID, "project") != null)
				$project = $this -> getEntry($bibtexID, "project");
		else $project = "";
		
		if ($this -> getEntry($bibtexID, "h5index") != null)
				$h5 = $this -> getEntry($bibtexID, "h5index");
		else $h5 = "";
		
		if ($this -> getEntry($bibtexID, "impact") != null)
				$impact = $this -> getEntry($bibtexID, "impact");
		else $impact = "";
		
		if ($this -> getEntry($bibtexID, "award") != null)
				$award = $this -> getEntry($bibtexID, "award");
		else $award = "";
		
		if ($this -> getEntry($bibtexID, "doi") != null)
				$doi = $this -> getEntry($bibtexID, "doi");
		else $doi = "";
		
		if ($this -> getEntry($bibtexID, "url") != null)
				$url = $this -> getEntry($bibtexID, "url");
		else $url = "";
		
		if ($this -> getEntry($bibtexID, "acceptrate") != null)
				$acceptrate = $this -> getEntry($bibtexID, "acceptrate");
		else $acceptrate = "";
		
		if ($this -> getEntry($bibtexID, "abstract") != null)
				$abstract = $this -> getEntry($bibtexID, "abstract");
		
		if ($this -> getEntry($bibtexID, "slidesurl") != null)
				$slidesurl = $this -> getEntry($bibtexID, "slidesurl");
		else $slidesurl = "";
		
		if ($this -> getEntry($bibtexID, "videourl") != null)
				$videourl = $this -> getEntry($bibtexID, "videourl");
		else $videourl = "";
		
		if ($this -> getEntry($bibtexID, "pubtype") != null)
				$pubtype = $this -> getEntry($bibtexID, "pubtype");
		
		if ($this -> getEntry($bibtexID, "pubcontext") != null)
				$pubcontext = $this -> getEntry($bibtexID, "pubcontext");
		else
			$pubcontext ="";
		
		if ($this -> getEntry($bibtexID, "series") != null)
				$series = $this -> getEntry($bibtexID, "series");
		
		if ($this -> getEntry($bibtexID, "booktitle") != null)
				$booktitle = $this -> getEntry($bibtexID, "booktitle");
		
		if ($this -> getEntry($bibtexID, "year") != null)
				$year = $this -> getEntry($bibtexID, "year");
		
		if ($this -> getEntry($bibtexID, "role") != null)
				$role = $this -> getEntry($bibtexID, "role");
		
		if ($this -> getEntry($bibtexID, "publisher") != null)
				$publisher = $this -> getEntry($bibtexID, "publisher");
		else
				$publisher = "";
		
		if ($publisher == "Association for Computing Machinery")
			$publisher = "ACM";
		
		
		//------------------------------------//
		
		$publicationCode = "<div class=\"pub-container container\">
			<p class= \"publication-citation col-lg-9 col-md-12 col-sm-12 col-xs-12\">" . $number . ". ";
		
		if ($award != null && $award == "Honorable Mention")
			$publicationCode .="<i class=\"fas fa-medal\"></i> ";
		
		if ($award != null && $award == "Student Game Design Competition, Audience Choice, 2nd Place")
			$publicationCode .="<i class=\"fas fa-trophy\"></i> ";
		
		$publicationCode .= $this -> getCitation($bibtexID,"SIGCHI",1,"Buruk");
		
		if ($acceptrate != "" || $impact != "" || $h5 != "" )
		$publicationCode .= " [";
		
		if ($award != null)
			$publicationCode .="<strong>". $award ."</strong>, ";
		
		if ($acceptrate != ""){
		$publicationCode .= "acceptance rate: " . $acceptrate . "%";
			if($impact != "" || $h5 != "")
			$publicationCode .=	", ";
		}
		
		if ($impact != ""){
		$publicationCode .= "impact factor: " . $impact;
			if($h5 != "")
			$publicationCode .=	", ";			
		}
		
		if ($h5 != "")
		$publicationCode .= "h5-index: " . $h5;
		
		if ($acceptrate != "" || $impact != "" || $h5 != "" )
		$publicationCode .= "]";
		
		$publicationCode .= "</p>";
		
		$publicationCode .= "<div class=\"col-lg-3 lg-pl-40 md-pl-10 sm-pl-10 xs-pl-10\">
				<div style=\"padding-left: 0px\">
				<button type=\"button\" class=\"btn pub-btn publication-button-citation pub-abs\" data-fancybox data-options='{\"src\": \"#" . $bibtexID . "Abs\", \"touch\": false}' ><i class=\"fas fa-align-justify\"></i></button>";
		
		if(file_exists("../Resources/Papers/" . $bibtexID . ".pdf")){				
		$publicationCode .="
		<button type=\"button\" class=\"btn pub-btn publication-button-pdf\" onclick=\"window.open('../Resources/Papers/" . $bibtexID . ".pdf')\"><i class=\"fas fa-file-pdf\"></i></button>";}
		
		if(!file_exists("../Resources/Papers/" . $bibtexID . ".pdf")){
		$publicationCode .="
		<button type=\"button\" class=\"btn pub-btn publication-button-pdf disabled\"><i class=\"fas fa-file-pdf\"></i></button>";}
		
		
		$publicationCode .="
		<button type=\"button\" class=\"btn pub-btn publication-button-citation\" data-fancybox data-options='{\"src\": \"#" . $bibtexID . "Cite\", \"touch\": false}' ><i class=\"fas fa-quote-right\"></i></button>";
		
		if ($doi != "")
		$publicationCode .=	"
		<button type=\"button\" class=\"btn pub-btn publication-button-webpage\" onclick=\"window.open('https://doi.org/" . $doi . "')\"><i class=\"fas fa-globe\"></i></button>";
		
		else if ($url != "")
		$publicationCode .=	"
		<button type=\"button\" class=\"btn pub-btn publication-button-webpage\" onclick=\"window.open('" . $url . "')\"><i class=\"fas fa-globe\"></i></button>";
		
		else
		$publicationCode .=	"
		<button type=\"button\" class=\"btn pub-btn publication-button-webpage disabled\"> <i class=\"fas fa-globe\"></i></button>";
		
		if ($project != "")
		$publicationCode .=	"
		<button type=\"button\" class=\"btn pub-btn publication-button-propage\"  style=\"padding-left: 10px\"onclick=\"window.open('../pages/" . $project . ".php#" . $bibtexID . "')\"><i class=\"fas fa-flask\"></i></button>
	  	</div></div>";
			
		else
		$publicationCode .=	"
		<button type=\"button\" class=\"btn pub-btn publication-button-propage disabled\"  style=\"padding-left: 10px\"><i class=\"fas fa-flask\"></i></button>
	  	</div></div>";
		
		$publicationCode .= $this -> produceCitationBox($bibtexID);
		
		$publicationCode .= "
					</div>

					<div style=\"display: none;\" id=\"" . $bibtexID . "Abs\">
						<div class=\"absContainer\">
							<div class=\"absContainer\">
							  <section class=\"col-lg-12\">
								<h3>" . $title . "</h3>
								</p> </section>
							</div>

							<div class=\"absContainer\">
							  <section class=\"col-lg-12\">
								<h4>Abstract</h4></p> </section>
							</div>


							<div class=\"absContainer\">
							  <section onClick=\"selectText(this.id)\" id=\"elementID\" class=\"col-lg-12\">" . $abstract . "</p> </section>
							</div>

						</div>	
					</div> 

				</div>
			</div> 

		";
		
		return $publicationCode;
		
		
	}
	
	public function publicationList($bibtexID, $number, $h5index, $impactfactor, $acceptancerate, $jufo){
		
		if ($this -> getEntry($bibtexID, "title") != null)
				$title = $this -> getEntry($bibtexID, "title");
		
		if ($this -> getEntry($bibtexID, "project") != null)
				$project = $this -> getEntry($bibtexID, "project");
		else $project = "";
		
		if ($this -> getEntry($bibtexID, "imagesmall") != null)
				$imagesmall = $this -> getEntry($bibtexID, "imagesmall");
		else $imagesmall = "";
		
		if ($this -> getEntry($bibtexID, "h5index") != null)
				$h5 = $this -> getEntry($bibtexID, "h5index");
		else $h5 = "";
		
		if ($this -> getEntry($bibtexID, "impact") != null)
				$impact = $this -> getEntry($bibtexID, "impact");
		else $impact = "";
		
		if ($this -> getEntry($bibtexID, "award") != null)
				$award = $this -> getEntry($bibtexID, "award");
		else $award = "";
		
		if ($this -> getEntry($bibtexID, "doi") != null)
				$doi = $this -> getEntry($bibtexID, "doi");
		else $doi = "";
		
		if ($this -> getEntry($bibtexID, "url") != null)
				$url = $this -> getEntry($bibtexID, "url");
		else $url = "";
		
		if ($this -> getEntry($bibtexID, "acceptrate") != null)
				$acceptrate = $this -> getEntry($bibtexID, "acceptrate");
		else $acceptrate = "";
		
		if ($this -> getEntry($bibtexID, "abstract") != null)
				$abstract = $this -> getEntry($bibtexID, "abstract");
		
		if ($this -> getEntry($bibtexID, "slidesurl") != null)
				$slidesurl = $this -> getEntry($bibtexID, "slidesurl");
		else $slidesurl = "";
		
		if ($this -> getEntry($bibtexID, "videourl") != null)
				$videourl = $this -> getEntry($bibtexID, "videourl");
		else $videourl = "";
		
		if ($this -> getEntry($bibtexID, "pubtype") != null)
				$pubtype = $this -> getEntry($bibtexID, "pubtype");
		
		if ($this -> getEntry($bibtexID, "pubcontext") != null)
				$pubcontext = $this -> getEntry($bibtexID, "pubcontext");
		else
			$pubcontext ="";
		
		if ($this -> getEntry($bibtexID, "series") != null)
				$series = $this -> getEntry($bibtexID, "series");
		
		if ($this -> getEntry($bibtexID, "booktitle") != null)
				$booktitle = $this -> getEntry($bibtexID, "booktitle");
		
		if ($this -> getEntry($bibtexID, "year") != null)
				$year = $this -> getEntry($bibtexID, "year");
		
		if ($this -> getEntry($bibtexID, "role") != null)
				$role = $this -> getEntry($bibtexID, "role");
		
		if ($this -> getEntry($bibtexID, "publisher") != null)
				$publisher = $this -> getEntry($bibtexID, "publisher");
		else
				$publisher = "";
		
		if ($publisher == "Association for Computing Machinery")
			$publisher = "ACM";
		
		if ($this -> getEntry($bibtexID, "category") != null)
				$category = $this -> getEntry($bibtexID, "category");
		else
				$category = "";
		
		$categoryPrefix = "";
		
		if ($category == "a1")
			$categoryPrefix = "AJA";
		
		if ($category == "a2")
			$categoryPrefix = "ACP";
		
		if ($category == "a3")
			$categoryPrefix = "AEA";
		
		if ($category == "a4")
			$categoryPrefix = "ABC";
		
		if ($category == "b1")
			$categoryPrefix = "BJA";
		
		if ($category == "b2")
			$categoryPrefix = "BCP";
		
		if ($category == "b3")
			$categoryPrefix = "BEA";
		
		if ($category == "b4")
			$categoryPrefix = "BBC";
		
		if ($category == "c1")
			$categoryPrefix = "CB";
		
		if ($category == "d1")
			$categoryPrefix = "DPR";
		
		if ($category == "e1")
			$categoryPrefix = "EPA";
		
		if ($category == "g1")
			$categoryPrefix = "GDD";
		
		
		
		//------------------------------------//
		
		$publicationCode = "<div class=\"pub-container container col-lg-12\">
			<p class= \"publication-list-citation\">" . $categoryPrefix . $number . ". ";
		
		$publicationCode .= $this -> getCitation($bibtexID,"SIGCHI",1,"Buruk");
		
		if (($acceptrate != "" && $acceptancerate == 1) || ($impact != "" && $impactfactor == 1) || ($h5 != "" && $h5index == 1))
		$publicationCode .= " [";
		
		if ($acceptrate != ""){
		$publicationCode .= "acceptance rate: " . $acceptrate . "%";
			if(($impact != "" && $impactfactor == 1) || ($h5 != "" && $h5index == 1))
			$publicationCode .=	", ";
		}
		
		if ($impact != "" && $impactfactor == 1){
		$publicationCode .= "impact factor: " . $impact;
			if($h5 != "" && $h5index == 1)
			$publicationCode .=	", ";			
		}
		
		if ($h5 != "" && $h5index == 1)
		$publicationCode .= "h5-index: " . $h5;
		
		if (($acceptrate != "" && $acceptancerate == 1) || ($impact != "" && $impactfactor == 1) || ($h5 != "" && $h5index == 1))
		$publicationCode .= "]";
		
		$publicationCode .= "</p> </div>";
		
		return $publicationCode;
		
		
	}
	
	public function createprojectpages($requestarray){//requestarray is only used if another functions calls for the array of projects
		
			if (file_exists("publications.bib"))
			$file = file ("publications.bib");
		
			else
				$file = file ("../publications.bib");

			$filestr = implode ("",$file);
			$splitbibtexentry = preg_split("/@/",$filestr);

			$projectarray = array();
			$pagearray = array();
			$projectname = "";
			$pagename = "";

		
		
			for ($i = 0; $i < sizeof($splitbibtexentry); $i++){
			
			$entry = $splitbibtexentry[$i];
			
			$strposbracket1 = strpos($entry, '{') + 1;
			$strposbracket2 = strpos($entry, ',') - $strposbracket1 ;
			$bibtextID = substr($entry, $strposbracket1, $strposbracket2);

			
			if ($this->getEntry($bibtextID, "project") != null){
				$pagename = $this->getEntry($bibtextID, "project");
				
				$projectname = $pagename;
				$projectname = str_replace("_", " ", $projectname);
				
			}
	
			
					
			
			
			if (!in_array($projectname, $projectarray) && $projectname != "gamedevelopment")
				array_push($projectarray, $projectname);
				
			if (!in_array($pagename, $pagearray) && $pagename != "gamedevelopment")
				array_push($pagearray, $pagename);
			
			}
		
		
		if (($requestarray != "projectArray") && ($requestarray != "pageArray")){ 
			for ($i = 1; $i < sizeof($pagearray); $i++){
				
				$pagecode = "<!DOCTYPE html>
					<html lang=\"en\">
					<head>
					<meta charset=\"utf-8\">
					<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">
					<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">
					<title>" . $projectarray[$i] . "</title>

					<!-- Bootstrap -->
					<link rel=\"stylesheet\" href=\"../css/bootstrap.css\">
					<style type=\"text/css\">
					@media (max-width: 1920px){
					}
					</style>

					<!-- HTML5 shim and Respond.js for IE8 support of HTML5 elements and media queries -->
					<!-- WARNING: Respond.js doesn't work if you view the page via file:// -->
					<!--[if lt IE 9]>
						  <script src=\"https://oss.maxcdn.com/html5shiv/3.7.2/html5shiv.min.js\"></script>
						  <script src=\"https://oss.maxcdn.com/respond/1.4.2/respond.min.js\"></script>
						<![endif]-->

					<link rel=\"stylesheet\" type=\"text/css\" href=\"../css/my.css\">
					<link rel=\"stylesheet\" type=\"text/css\" href=\"../css/less-space.css\">
					<link rel=\"stylesheet\" type=\"text/css\" href=\"../css/less-space.css\">
					<link href=\"../jQueryAssets/jquery.ui.core.min.css\" rel=\"stylesheet\" type=\"text/css\">
					<link href=\"../jQueryAssets/jquery.ui.theme.min.css\" rel=\"stylesheet\" type=\"text/css\">
					<script src=\"../jQueryAssets/jquery-1.11.1.min.js\"></script>

					<script type=\"text/javascript\" src=\"http://code.jquery.com/jquery-3.2.1.min.js\"></script>	
					<script type=\"text/javascript\" src=\"/fancybox/lib/jquery.mousewheel-3.0.6.pack.js\"></script>

					<link rel=\"stylesheet\" href =\"../js/cite-box/fancy-box/dist/jquery.fancybox.min.css\" type= \"text/css\" media=\"screen\" />
					<script type = \"text/javascript\" src=\"../js/cite-box/fancy-box/dist/jquery.fancybox.min.js\"></script>
					<script src=\"https://kit.fontawesome.com/3a09c9e27b.js\" crossorigin=\"anonymous\"></script>

				<script>
						function copy(id, button){
							var str = document.getElementById(id).innerText;

							const listener = function(ev) {
								ev.preventDefault();
								ev.clipboardData.setData('text/html', str);
								ev.clipboardData.setData('text/plain', str);
							};
							  document.addEventListener('copy', listener);
							  document.execCommand('copy');
							  document.removeEventListener('copy', listener);



							// visual feedback that task is completed
							var buttonText = document.getElementById(button);
							var label = buttonText.innerHTML;
							var message = \"<i>Copied:</i><br>\" + label;
							buttonText.innerHTML = message;

							if (button == 'BibTeXbutton'){
								setTimeout(() => {
								buttonText.innerHTML = label;
								}, 1500);
							}

							else {
							setTimeout(() => {
								buttonText.innerHTML = label;
							}, 1500);}
						}

	
	
						function copySelected(id, button){
							selectText(id);
							var selectedText = \"\";
							selectedText = window.getSelection().toString();
							if (selectedText.length > 0){
								document.execCommand(\"copy\");
							}

							// visual feedback that task is completed
							var buttonText = document.getElementById(button);
							var label = buttonText.innerHTML;
							var message = \"<i>Copied:</i><br>\" + label;
							buttonText.innerHTML = message;

							if (button == 'BibTeXbutton'){
								setTimeout(() => {
								buttonText.innerHTML = label;
								}, 1500);
							}

							else {
							setTimeout(() => {
								buttonText.innerHTML = label;
							}, 1500);}

						}
	
						function selectText(id){
							var sel, range, str;
							var el = document.getElementById(id); //get element id
							if (window.getSelection && document.createRange) { //Browser compatibility
							  sel = window.getSelection();

							  if (sel.toString().length > 0)
								  sel.removeAllRanges(); //remove all ranges from selection

							  if(sel.toString() == ''){ //no text selection
									range = document.createRange(); //range object
									range.selectNodeContents(el); //sets Range
									sel.removeAllRanges();
									sel.addRange(range);//add Range to a Selection.
							  }

							}else if (document.selection) { //older ie
								sel = document.selection.createRange();
								if(sel.text == ''){ //no text selection
									range = document.body.createTextRange();//Creates TextRange object
									range.moveToElementText(el);//sets Range
									range.select(); //make selection.
									}
								}
							}
						</script>

						<?php
							require(__DIR__.\"/../object.inc\");
							\$creator = new bibParser();
						?>

					</head>
					</h>

						<?php
							echo \$creator -> navbar(0);
						?>



						<!-- /.navbar-collapse -->

					  <!-- /.container-fluid -->

					<body>



					<div class=\"container projectTitleFrame\"><projectPageTitles class=\"col-lg-8 col-md-8 col-sm-12 col-xs-12\">" . $projectarray[$i] . "</projectPageTitles ></div>

						<?php 	
						 \$array = \$creator -> getContent (15,\"paper\",\"project\",\"" . $pagearray[$i] . "\",0,1);
						  for (\$i=0; \$i<sizeof(\$array); \$i++){
							  echo \$creator -> publicationSummary(\$array[\$i]);
						  }


						?>


					<hr>	
						<?php
							echo \$creator -> footer();
						?>
					<hr>





					<!-- jQuery (necessary for Bootstrap's JavaScript plugins) --> 
					<script src=\"../js/jquery-1.11.3.min.js\"></script> 
					<!-- Include all compiled plugins (below), or include individual files as needed --> 
					<script src=\"../js/bootstrap.js\"></script>
					<script type =\"text/javascript\">


						</script>
					<script>
					  (function(i,s,o,g,r,a,m){i['GoogleAnalyticsObject']=r;i[r]=i[r]||function(){
					  (i[r].q=i[r].q||[]).push(arguments)},i[r].l=1*new Date();a=s.createElement(o),
					  m=s.getElementsByTagName(o)[0];a.async=1;a.src=g;m.parentNode.insertBefore(a,m)
					  })(window,document,'script','https://www.google-analytics.com/analytics.js','ga');

					  ga('create', 'UA-45809350-2', 'auto');
					  ga('send', 'pageview');

					</script>	
					</body>
					</html>";

				$filename = "pages/" . $pagearray[$i] . ".php";

				$fp = fopen($filename,'w');

				fwrite($fp, $pagecode);

				fclose($fp);

			}
		}
		
			
		
		if ($requestarray == "projectArray")
			return $projectarray;
		
		if ($requestarray == "pageArray")
			return $pagearray;
		
		
		
	}
	
	public function navbar($mainpage){
		
	
		$projectarray = $this -> createprojectpages ("projectArray"); //create project pages function also creates an array for project names to be fetched by outside functions
		
		$pagearray = $this -> createprojectpages ("pageArray"); //create project pages function also creates an array for project names to be fetched by outside functions
		
		$navbarcode = "<nav class=\"navbar navbar-inverse";
		
		if (!$mainpage)
			$navbarcode .= " navbar-fixed-top\"";
		
		$navbarcode .= " id=\"navbar\">
				   <div class=\"container\"><div class=\"container-fluid\">
					<!-- Brand and toggle get grouped for better mobile display -->
					<div class=\"navbar-header\">
					  <button type=\"button\" class=\"navbar-toggle collapsed\" data-toggle=\"collapse\" data-target=\"#inverseNavbar1\" aria-expanded=\"false\"><span class=\"sr-only\">Toggle navigation</span><span class=\"icon-bar\"></span><span class=\"icon-bar\"></span><span class=\"icon-bar\"></span></button>
					  <a class=\"navbar-brand\" href=\"";
		if (!$mainpage)
			$navbarcode .= "../";
		
		$navbarcode .= "index.php\">Oğuz 'Oz' Buruk</a></div>
					<!-- Collect the nav links, forms, and other content for toggling -->

					<div class=\"collapse navbar-collapse\" id=\"inverseNavbar1\">
					  <ul class=\"nav navbar-nav\">
					   <li class=\"dropdown\"><a href=\"#\" class=\"dropdown-toggle\" data-toggle=\"dropdown\" role=\"button\" aria-haspopup=\"true\" aria-expanded=\"false\">Research<span class=\"caret\"></span></a>
						  <ul class=\"dropdown-menu\">";
		
		for ($i = 1; $i < sizeof($projectarray); $i++){
		
			$navbarcode .= "<li><a href=\"";
			
			if (!$mainpage)
			$navbarcode .= "../";
				
			$navbarcode .=	"pages/" . $pagearray[$i] . ".php\">" . $projectarray[$i] ."</a></li>";
			
			if ($i < sizeof($projectarray)-1)
				$navbarcode .= "<li role=\"separator\" class=\"divider\"></li>"; 
		
		}
		
		if (!$mainpage){
							
		$navbarcode .="</ul>
						</li>
				<!--        <li class=\"active\"><a href=\"#\">Link<span class=\"sr-only\">(current)</span></a></li>-->
						  <li><a href=\"../pages/publications.php\">Publications</a></li>
						<li><a href=\"../pages/gamedevelopment.html\">Game Development</a></li>
						<li><a href=\"../pages/bio.html\">Bio</a></li>
						<li><a href=\"../pages/funactivites.html\">Fun Activities</a></li>

					  </ul>
					  <ul class=\"nav navbar-nav navbar-right\">
						<li><a target=\"_blank\" href=\"../Resources/OguzTuranBuruk_CV.pdf\">CV</a></li>";
			}
		
		
		else if ($mainpage){
							
		$navbarcode .="</ul>
						</li>
				<!--        <li class=\"active\"><a href=\"#\">Link<span class=\"sr-only\">(current)</span></a></li>-->
						  <li><a href=\"pages/publications.php\">Publications</a></li>
						<li><a href=\"pages/gamedevelopment.html\">Game Development</a></li>
						<li><a href=\"pages/bio.html\">Bio</a></li>
						<li><a href=\"pages/funactivites.html\">Fun Activities</a></li>

					  </ul>
					  <ul class=\"nav navbar-nav navbar-right\">
						<li><a target=\"_blank\" href=\"Resources/OguzTuranBuruk_CV.pdf\">CV</a></li>";
			}
		
		
		
		
		$navbarcode .= "<li><a target=\"_blank\" href=\"https://scholar.google.com/citations?user=U-SZUDQAAAAJ&hl=en\">Google Scholar</a></li>
						<li class=\"dropdown\"><a href=\"#\" class=\"dropdown-toggle\" data-toggle=\"dropdown\" role=\"button\" aria-haspopup=\"true\" aria-expanded=\"false\">Contact<span class=\"caret\"></span></a>
						  <ul class=\"dropdown-menu\">
							<li><a href=\"mailto:oguzturanburuk@gmail.com\">oguzturanburuk@gmail.com</a></li>
							<li><a target=\"_blank\" href=\"https://twitter.com/lustintirauel\">Twitter</a></li>
							<li><a target=\"_blank\" href=\"https://www.facebook.com/oguz.buruk\">Facebook</a></li>
						  </ul>
						</li>
					  </ul>
					</div>
					<!-- /.navbar-collapse -->
				  </div></div>

				  <!-- /.container-fluid -->
				</nav>";
		
		
		return $navbarcode;
		
		
	}
	
	public function footer(){
		
			
		$footercode = "<footer class=\"text-center\">
		  <div class=\"container\">
			<p class=\"text-center grayed\">Oz Research 2024 All Rights Reserved | <a href=\"mailto:oguzturanburuk@gmail.com\">oguzturanburuk@<span style=\"display:none;\">ignoreme-</span>gmail.com</a> | <a target=\"_blank\" href=\"https://research.tuni.fi/challenge/\">Gameful Futures Lab, Research Centre of Gameful Realities</a> | Tampere University,  Kalevantie 4, Tampere / Finland</p>
		  </div>
		</footer>";
		
	
		return $footercode;
		
		
	}

	public function letstest ($message){
		print_r("you are running a function now with a message of " . $message);


	}

}

?>