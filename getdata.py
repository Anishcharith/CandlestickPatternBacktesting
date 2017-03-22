import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import datetime as dt
from matplotlib import style
import pandas_datareader.data as web
style.use('ggplot')
scrip=raw_input('Enter YAHOO Finance tickr symbol of Company')
start=input('Enter starting Year, Month and Date')
start=dt.datetime(start/10000,(start/100)%100,start%100)
end=input('Enter ending Year, Month and Date')
end=dt.datetime(end/10000,(end/100)%100,end%100)
df=web.DataReader(scrip,'yahoo',start,end)
print df.head()
df.to_csv(scrip)


