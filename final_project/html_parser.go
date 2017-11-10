package main

import (
	"fmt"

	"github.com/PuerkitoBio/goquery"
)

func getSiteList() [5]string {
	var sites [5]string;
	sites[0] = "http://calvinknights.com/sports/msoc/2017-18/schedule" ;
	sites[1] = "http://athletics.hope.edu/sports/msoc/2017-18/schedule";
	sites[2] = "http://hornets.kzoo.edu/sports/msoc/2017-18/schedule";
	sites[3] = "http://www.adrianbulldogs.com/sports/m-soccer/2017-18/schedule";
	sites[4] = "http://www.gobrits.com/sports/msoc/2017-18/schedule";

	return sites;
}

func main() {
	sites := getSiteList()

	for _, site := range sites {
		fmt.Printf("Site: %s\n", site)
		doc, err := goquery.NewDocument(site)

		if (err != nil) {
			fmt.Printf("Fatal error: %s\n", err);
		}

		fmt.Println("Here is the doc:", doc);
	}
}