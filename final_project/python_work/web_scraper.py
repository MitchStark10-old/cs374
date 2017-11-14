import urllib.request as urllib2
from bs4 import BeautifulSoup
import re

def getSiteList():
    sites = []
    sites.append("http://calvinknights.com/sports/msoc/2017-18/schedule")
    sites.append("http://athletics.hope.edu/sports/msoc/2017-18/schedule")
    sites.append("http://hornets.kzoo.edu/sports/msoc/2017-18/schedule")
    sites.append("http://www.adrianbulldogs.com/sports/m-soccer/2017-18/schedule")
    sites.append("http://www.gobrits.com/sports/msoc/2017-18/schedule")
    return sites


def getSchedule(site):
    page = urllib2.urlopen(site)
    page_soup = BeautifulSoup(page, "html.parser")
    game_links = page_soup.find_all("a", href=re.compile("boxscores"))
    print(game_links)

site_list = getSiteList()
for site in site_list:
    getSchedule(site)