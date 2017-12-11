package main

import (
	"fmt"
	"strings"
	"sync"
	"time"
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
	start_time := time.Now();
	sites := getSiteList();
	mux := &sync.Mutex{};
	var game_links []string;
	var wg sync.WaitGroup;

	for _, site := range sites {
		wg.Add(1);
		go func(site string) {
			links := getLinks(site, &wg);
			mux.Lock();
			game_links = append(game_links, links...);
			mux.Unlock();
		}(site)
	}

	wg.Wait();

	for _, link := range game_links {
        if (strings.Contains(link, "wbkb")) {
            continue;
        }
		wg.Add(1);
		go findAndPrintScores(link, &wg);
	}
	wg.Wait();
	elapsed := time.Since(start_time);

	fmt.Println("Execution time:", elapsed);
	fmt.Printf("Found [%d] games\n", len(game_links));
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
			//fmt.Println("Found link:", strings.Replace(site, "/sports/msoc/2017-18/schedule", "", -1) + href);
			main_site := strings.Replace(site, "/sports/msoc/2017-18/schedule", "", -1)
			main_site = strings.Replace(main_site, "/sports/m-soccer/2017-18/schedule", "", -1)
			hrefs = append(hrefs,  main_site + href);
		}
	})

	return hrefs;
}

func findAndPrintScores(game_link string, wg *sync.WaitGroup) {
	defer wg.Done();

	print_text := "Link: " + game_link + "\n";
	doc, err := goquery.NewDocument(game_link);

	if (err != nil) {
		fmt.Printf("Fatal error: %s\n", err);
		return;
	}

	var line_score *goquery.Selection = doc.Find(".linescore");
	line_score.Find("tr").Each(func(i int, s *goquery.Selection) {
		if (i != 0) {
			name := standardizeSpaces(s.Find("th").First().Text());
			score := s.Find(".score.total").Text();

			print_text += name + " - " + score + "\n"
			//fmt.Printf("%s - %s\n\n", name, score)
		}
	})
	// var winner *goquery.Selection = doc.Find(".winner");
	// var winner_name = standardizeSpaces(winner.Find("th").First().Text());
	// var winner_score = winner.Find(".score.total").Text();

	// var loser *goquery.Selection = doc.Find(".loser");
	// var loser_name = standardizeSpaces(loser.Find("th").First().Text());
	// var loser_socre = loser.Find(".score.total").Text();


	// fmt.Printf("\n%s - %s\n", winner_name, winner_score);
	// fmt.Printf("%s - %s\n\n", loser_name, loser_socre);
	fmt.Println(print_text);
}

func standardizeSpaces(s string) string {
    return strings.Join(strings.Fields(s), " ")
}
