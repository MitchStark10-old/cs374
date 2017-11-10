package main

import (
	"fmt"
	"strings"
	"sync"
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
	var game_links []string;
	var wg sync.WaitGroup;

	for _, site := range sites {
		wg.Add(1);
		go func() {
			game_links = append(game_links, getLinks(site, &wg)...);
		}()
	}

	wg.Wait();	

	for _, link := range game_links {
		fmt.Println("Link:", link);
	}
}

func getLinks(site string, wg *sync.WaitGroup) []string {
	defer wg.Done();
	var hrefs []string;

	doc, err := goquery.NewDocument(site)

	if (err != nil) {
		fmt.Printf("Fatal error: %s\n", err);
		return hrefs;
	}

	doc.Find("a").Each(func(i int, s *goquery.Selection) {
		href, _ := s.Attr("href");

		if (strings.Contains(href, "boxscores")) {
			//fmt.Println("Found link:", site + href);
			hrefs = append(hrefs, site + href);
		}
	})

	return hrefs;
}