#!/usr/bin/env python3
import glob
import heapq
import datetime
import tqdm
import numpy as np
import databento as db
import databento_dbn as dbn
import databento_classes

def parse_folder(data_path = "../data/databento/es/ftp.databento.com/E8XGYL35/GLBX-20241008-8PTR93CRA9/"):
    for file in sorted(glob.glob(data_path+'*.zst')):
        for result in parse_file(file):
            yield result

def parse_file(file = "../data/databento/es/ftp.databento.com/E8XGYL35/GLBX-20241008-8PTR93CRA9/glbx-mdp3-20240904.mbo.dbn.zst"):
    data = db.DBNStore.from_file(file)

    market = databento_classes.Market()

    instrument_map = db.common.symbology.InstrumentMap()
    instrument_map.insert_metadata(data.metadata)

    for mbo in data:
        market.apply(mbo)

        instrument = instrument_map.resolve(mbo.instrument_id, datetime.datetime.fromtimestamp(mbo.ts_recv*1e-9).date())
        yield (mbo, market, instrument)
        """
        print([str(o) for l in market.get_books_by_pub(mbo.instrument_id)[1].bids.values() for o in l.orders])
        # If it's the last update in an event, print the state of the aggregated book
        if mbo.flags & db.RecordFlags.F_LAST:
            symbol = (
                instrument_map.resolve(mbo.instrument_id, mbo.pretty_ts_recv.date())
                or ""
            )
            print(f"{symbol} Aggregated BBO | {mbo.pretty_ts_recv}")
            best_bid, best_offer = market.aggregated_bbo(mbo.instrument_id)
            print(f"    {best_offer}")
            print(f"    {best_bid}")
        """

def interleave(generators):
    mbos_queue = []
    for g in generators:
        try:
            result = next(g)
            if result:
                mbo, market, instrument = result
                heapq.heappush(mbos_queue, (mbo.ts_recv, mbo.ts_event, mbo, market, instrument, g))
        except StopIteration:
            continue
    while mbos_queue:
        ts, ts_event, mbo, market, instrument, g = heapq.heappop(mbos_queue)
        yield mbo, market, instrument
        try:
            result = next(g)
            if result:
                mbo, market, instrument = result
                heapq.heappush(mbos_queue, (mbo.ts_recv, mbo.ts_event, mbo, market, instrument, g))
        except StopIteration:
            continue

def write_csv(generator, output_file):
    with open(output_file,'w') as f:
        f.write('instrument,timestamp,action,side,size,price,ts_delta,bq,bp,aq,ap,is_last\n')
        for result in generator:
            mbo, market, instrument = result
            try:
                bid,ask = market.aggregated_bbo(mbo.instrument_id)
                data = [instrument, mbo.ts_recv * 1e-9, mbo.action, mbo.side, mbo.size, mbo.price, mbo.ts_in_delta, bid.size if bid else np.nan, bid.price/dbn.FIXED_PRICE_SCALE if bid else np.nan, ask.size if ask else np.nan, ask.price/dbn.FIXED_PRICE_SCALE if ask else np.nan, bool(mbo.flags & db.RecordFlags.F_LAST)]
                f.write(','.join(map(str,data))+'\n')
            except Exception as e:
                print('Exception: ', repr(e), mbo, bid, ask)

if __name__ == "__main__":
    folders = ["../data/databento/mes/ftp.databento.com/E8XGYL35/GLBX-20241008-PP9KBLT3CX/", "../data/databento/es/ftp.databento.com/E8XGYL35/GLBX-20241008-8PTR93CRA9/"]
    filenames = set([file.split('/')[-1] for folder in folders for file in glob.glob(f'{folder}/*.zst')])
    print(filenames)
    for filename in tqdm.tqdm(filenames):
        mbos = interleave([parse_file(folder+filename) for folder in folders])
        write_csv(mbos, '../output/databento/'+filename.split('.')[0]+'.csv')
