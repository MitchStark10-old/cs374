import urllib.request as urllib2
from bs4 import BeautifulSoup
from multiprocessing import Pool
import time
import re

def getSiteList():
    sites = []
    sites.append("http://calvinknights.com/sports/msoc/2017-18/schedule")
    sites.append("http://athletics.hope.edu/sports/msoc/2017-18/schedule")
    sites.append("http://hornets.kzoo.edu/sports/msoc/2017-18/schedule")
    sites.append("http://www.adrianbulldogs.com/sports/m-soccer/2017-18/schedule")
    sites.append("http://www.gobrits.com/sports/msoc/2017-18/schedule")
    return sites

def getSoup(site):
    page = urllib2.urlopen(site)
    return BeautifulSoup(page, "html.parser")

def getSchedule(site):
    schedule = []
    page_soup = getSoup(site)
    game_links = page_soup.find_all("a", href=re.compile("boxscores"))
    for link in game_links:
        new_game = str(site).replace("/sports/msoc/2017-18/schedule", "").replace("/sports/m-soccer/2017-18/schedule", "") + link['href']
        schedule.append(new_game)
    return list(set(schedule))

def getScore(game_link):
    summary_str = ""
    page_soup = getSoup(game_link)
    line_score = page_soup.find("div", { "class" : "linescore" })
    if line_score is None:
        return

    print(game_link)        
    table_items = line_score.find_all("tr")

    for item in [table_items[1], table_items[2]]:
        team_name = item.find("th").contents[0].strip()
        if team_name == "":
            team_name = item.find("a").contents[0].strip()
        summary_str += team_name + \
            " - " + item.find("td", { "class" : "score total"} ).contents[0] + "\n"
    
    print(summary_str)

start = time.time()

site_list = getSiteList()
pool = Pool()
game_links = pool.map(getSchedule, site_list)
flat_game_links = [item for sublist in game_links for item in sublist]
pool.map(getScore, flat_game_links)

end = time.time()
print("Execution time: " + str(end - start))
print("Found [" + str(len(flat_game_links)) + "] games")