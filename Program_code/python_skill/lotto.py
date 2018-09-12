#!/usr/bin/python

import requests
import re
from bs4 import BeautifulSoup

date = []
perios = []
#lato_page = 'http://www.taiwanlottery.com.tw/lotto/superlotto638/history.aspx'
lato_page = 'http://www.taiwanlottery.com.tw/index_new.aspx'

page = requests.get(lato_page)

if page.status_code == requests.codes.ok:
	soup = BeautifulSoup(page.text, 'html.parser')

#name_box = soup.find_all('h1',attrs={'class':'font_red18 tx_md'})

for s in soup.select('span'):
	print(s.text)
