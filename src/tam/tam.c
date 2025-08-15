#include "../onesparse.h"

#define TAMLOG(fmt, ...) elog(INFO, "%s: " fmt, __func__, ##__VA_ARGS__)

static const char *
scan_direction_to_str(ScanDirection dir)
{
    if (ScanDirectionIsForward(dir))
        return "Forward";
    if (ScanDirectionIsBackward(dir))
        return "Backward";
    return "Other";
}

char *
format_tid_alloc(ItemPointer tid)
{
    char *s = palloc(32);
    if (!ItemPointerIsValid(tid))
        strcpy(s, "NULL");
    else
        snprintf(s, 32, "(%u,%u)",
                 ItemPointerGetBlockNumber(tid),
                 ItemPointerGetOffsetNumber(tid));
    return s;
}

static TableAmRoutine *onesparse_routine = NULL;
static const TableAmRoutine *underlying = NULL;

static const TupleTableSlotOps *onesparse_slot_callbacks(Relation rel)
{
    TAMLOG("rel=%s", RelationGetRelationName(rel));
    return underlying->slot_callbacks(rel);
}

static TableScanDesc onesparse_scan_begin(Relation rel, Snapshot snapshot,
                                          int nkeys, struct ScanKeyData *key,
                                          ParallelTableScanDesc pscan,
                                          uint32 flags)
{
    TAMLOG("rel=%s, snapshot=%p, nkeys=%d, pscan=%p, flags=0x%x",
         RelationGetRelationName(rel), (void *) snapshot, nkeys, (void *) pscan, flags);
    return underlying->scan_begin(rel, snapshot, nkeys, key, pscan, flags);
}

static void onesparse_scan_end(TableScanDesc scan)
{
    TAMLOG("scan=%p", (void *) scan);
    underlying->scan_end(scan);
}

static void onesparse_scan_rescan(TableScanDesc scan, struct ScanKeyData *key,
                                  bool set_params, bool allow_strat,
                                  bool allow_sync, bool allow_pagemode)
{
    TAMLOG("scan=%p, set_params=%d, allow_strat=%d, allow_sync=%d, allow_pagemode=%d",
         (void *) scan, (int) set_params, (int) allow_strat, (int) allow_sync, (int) allow_pagemode);
    underlying->scan_rescan(scan, key, set_params, allow_strat, allow_sync, allow_pagemode);
}

static bool onesparse_scan_getnextslot(TableScanDesc scan,
                                       ScanDirection direction,
                                       TupleTableSlot *slot)
{
    TAMLOG("scan=%p, direction=%s, slot=%p", (void *) scan, scan_direction_to_str(direction), (void *) slot);
    return underlying->scan_getnextslot(scan, direction, slot);
}

static void onesparse_scan_set_tidrange(TableScanDesc scan,
                                        ItemPointer mintid,
                                        ItemPointer maxtid)
{
    TAMLOG("scan=%p, mintid=%s, maxtid=%s",
         (void *) scan,
         format_tid_alloc(mintid),
         format_tid_alloc(maxtid));
    underlying->scan_set_tidrange(scan, mintid, maxtid);
}

static bool onesparse_scan_getnextslot_tidrange(TableScanDesc scan,
                                                ScanDirection direction,
                                                TupleTableSlot *slot)
{
    TAMLOG("scan=%p, direction=%s, slot=%p (tidrange variant)",
         (void *) scan, scan_direction_to_str(direction), (void *) slot);
    return underlying->scan_getnextslot_tidrange(scan, direction, slot);
}

static Size onesparse_parallelscan_estimate(Relation rel)
{
    TAMLOG("rel=%s", RelationGetRelationName(rel));
    return underlying->parallelscan_estimate(rel);
}

static Size onesparse_parallelscan_initialize(Relation rel, ParallelTableScanDesc pscan)
{
    TAMLOG("rel=%s, pscan=%p", RelationGetRelationName(rel), (void *) pscan);
    return underlying->parallelscan_initialize(rel, pscan);
}

static void onesparse_parallelscan_reinitialize(Relation rel, ParallelTableScanDesc pscan)
{
    TAMLOG("rel=%s, pscan=%p", RelationGetRelationName(rel), (void *) pscan);
    underlying->parallelscan_reinitialize(rel, pscan);
}

static IndexFetchTableData *onesparse_index_fetch_begin(Relation rel)
{
    TAMLOG("rel=%s", RelationGetRelationName(rel));
    return underlying->index_fetch_begin(rel);
}

static void onesparse_index_fetch_reset(IndexFetchTableData *data)
{
    TAMLOG("data=%p", (void *) data);
    underlying->index_fetch_reset(data);
}

static void onesparse_index_fetch_end(IndexFetchTableData *data)
{
    TAMLOG("data=%p", (void *) data);
    underlying->index_fetch_end(data);
}

static bool onesparse_index_fetch_tuple(IndexFetchTableData *scan,
                                        ItemPointer tid, Snapshot snapshot,
                                        TupleTableSlot *slot,
                                        bool *call_again, bool *all_dead)
{
    TAMLOG("scan=%p, tid=%s, snapshot=%p, slot=%p", (void *) scan,
         format_tid_alloc(tid),
         (void *) snapshot, (void *) slot);
    return underlying->index_fetch_tuple(scan, tid, snapshot, slot, call_again, all_dead);
}

static bool onesparse_tuple_fetch_row_version(Relation rel, ItemPointer tid,
                                              Snapshot snapshot, TupleTableSlot *slot)
{
    TAMLOG("rel=%s, tid=%s, snapshot=%p, slot=%p", RelationGetRelationName(rel),
         format_tid_alloc(tid),
         (void *) snapshot, (void *) slot);
    return underlying->tuple_fetch_row_version(rel, tid, snapshot, slot);
}

static bool onesparse_tuple_tid_valid(TableScanDesc scan, ItemPointer tid)
{
    TAMLOG("scan=%p, tid=%s", (void *) scan,
         format_tid_alloc(tid));
    return underlying->tuple_tid_valid(scan, tid);
}

static void onesparse_tuple_get_latest_tid(TableScanDesc scan, ItemPointer tid)
{
    TAMLOG("scan=%p, tid=%s", (void *) scan,
         format_tid_alloc(tid));
    underlying->tuple_get_latest_tid(scan, tid);
}

static bool onesparse_tuple_satisfies_snapshot(Relation rel,
                                               TupleTableSlot *slot,
                                               Snapshot snapshot)
{
    TAMLOG("rel=%s, slot=%p, snapshot=%p", RelationGetRelationName(rel), (void *) slot, (void *) snapshot);
    return underlying->tuple_satisfies_snapshot(rel, slot, snapshot);
}

static TransactionId onesparse_index_delete_tuples(Relation rel, TM_IndexDeleteOp *delstate)
{
    TAMLOG("rel=%s, delstate=%p", RelationGetRelationName(rel), (void *) delstate);
    return underlying->index_delete_tuples(rel, delstate);
}

static void onesparse_tuple_insert(Relation rel, TupleTableSlot *slot,
                                   CommandId cid, int options,
                                   struct BulkInsertStateData *bistate)
{
    TAMLOG("rel=%s, cid=%u, options=0x%x, slot=%p", RelationGetRelationName(rel), cid, options, (void *) slot);
    underlying->tuple_insert(rel, slot, cid, options, bistate);
}

static void onesparse_tuple_insert_speculative(Relation rel, TupleTableSlot *slot,
                                               CommandId cid, int options,
                                               struct BulkInsertStateData *bistate,
                                               uint32 specToken)
{
    TAMLOG("rel=%s, cid=%u, options=0x%x, specToken=%u, slot=%p",
         RelationGetRelationName(rel), cid, options, specToken, (void *) slot);
    underlying->tuple_insert_speculative(rel, slot, cid, options, bistate, specToken);
}

static void onesparse_tuple_complete_speculative(Relation rel,
                                                 TupleTableSlot *slot,
                                                 uint32 specToken,
                                                 bool succeeded)
{
    TAMLOG("rel=%s, specToken=%u, succeeded=%d, slot=%p",
         RelationGetRelationName(rel), specToken, (int) succeeded, (void *) slot);
    underlying->tuple_complete_speculative(rel, slot, specToken, succeeded);
}

static void onesparse_multi_insert(Relation rel, TupleTableSlot **slots,
                                   int nslots, CommandId cid, int options,
                                   struct BulkInsertStateData *bistate)
{
    TAMLOG("rel=%s, nslots=%d, cid=%u, options=0x%x", RelationGetRelationName(rel), nslots, cid, options);
    underlying->multi_insert(rel, slots, nslots, cid, options, bistate);
}

static TM_Result onesparse_tuple_delete(Relation rel, ItemPointer tid,
                                        CommandId cid, Snapshot snapshot,
                                        Snapshot crosscheck, bool wait,
                                        TM_FailureData *tmfd, bool changingPart)
{
    TAMLOG("rel=%s, tid=%s, cid=%u, wait=%d, changingPart=%d",
         RelationGetRelationName(rel),
         format_tid_alloc(tid),
         cid, (int) wait, (int) changingPart);
    return underlying->tuple_delete(rel, tid, cid, snapshot, crosscheck, wait, tmfd, changingPart);
}

static TM_Result onesparse_tuple_update(Relation rel, ItemPointer otid,
                                        TupleTableSlot *slot, CommandId cid,
                                        Snapshot snapshot, Snapshot crosscheck,
                                        bool wait, TM_FailureData *tmfd,
                                        LockTupleMode *lockmode,
                                        TU_UpdateIndexes *update_indexes)
{
    TAMLOG("rel=%s, otid=%s, cid=%u, wait=%d, lockmode=%d, slot=%p",
         RelationGetRelationName(rel),
         format_tid_alloc(otid),
         cid, (int) wait, lockmode ? *lockmode : -1, (void *) slot);
    return underlying->tuple_update(rel, otid, slot, cid, snapshot, crosscheck, wait, tmfd, lockmode, update_indexes);
}

static TM_Result onesparse_tuple_lock(Relation rel, ItemPointer tid,
                                      Snapshot snapshot, TupleTableSlot *slot,
                                      CommandId cid, LockTupleMode mode,
                                      LockWaitPolicy wait_policy, uint8 flags,
                                      TM_FailureData *tmfd)
{
    TAMLOG("rel=%s, tid=%s, cid=%u, mode=%d, wait_policy=%d, flags=0x%x",
         RelationGetRelationName(rel),
         format_tid_alloc(tid),
         cid, mode, wait_policy, flags);
    return underlying->tuple_lock(rel, tid, snapshot, slot, cid, mode, wait_policy, flags, tmfd);
}

static void onesparse_finish_bulk_insert(Relation rel, int options)
{
    TAMLOG("rel=%s, options=0x%x", RelationGetRelationName(rel), options);
    underlying->finish_bulk_insert(rel, options);
}

static void onesparse_relation_set_new_filelocator(Relation rel,
                                                   const RelFileLocator *newrlocator,
                                                   char persistence,
                                                   TransactionId *freezeXid,
                                                   MultiXactId *minmulti)
{
    TAMLOG("rel=%s, persistence=%c, newrlocator=%p", RelationGetRelationName(rel), persistence, (void *) newrlocator);
    underlying->relation_set_new_filelocator(rel, newrlocator, persistence, freezeXid, minmulti);
}

static void onesparse_relation_nontransactional_truncate(Relation rel)
{
    TAMLOG("rel=%s", RelationGetRelationName(rel));
    underlying->relation_nontransactional_truncate(rel);
}

static void onesparse_relation_copy_data(Relation rel, const RelFileLocator *newrlocator)
{
    TAMLOG("rel=%s, newrlocator=%p", RelationGetRelationName(rel), (void *) newrlocator);
    underlying->relation_copy_data(rel, newrlocator);
}

static void onesparse_relation_copy_for_cluster(Relation OldTable, Relation NewTable,
                                                Relation OldIndex, bool use_sort,
                                                TransactionId OldestXmin,
                                                TransactionId *xid_cutoff,
                                                MultiXactId *multi_cutoff,
                                                double *num_tuples,
                                                double *tups_vacuumed,
                                                double *tups_recently_dead)
{
    TAMLOG("OldTable=%s, NewTable=%s, use_sort=%d", RelationGetRelationName(OldTable),
         RelationGetRelationName(NewTable), (int) use_sort);
    underlying->relation_copy_for_cluster(OldTable, NewTable, OldIndex,
                                          use_sort, OldestXmin, xid_cutoff,
                                          multi_cutoff, num_tuples,
                                          tups_vacuumed, tups_recently_dead);
}

static void onesparse_relation_vacuum(Relation rel, struct VacuumParams params,
                                      BufferAccessStrategy bstrategy)
{
    TAMLOG("rel=%s, params=%p, bstrategy=%p", RelationGetRelationName(rel), &params, (void *) bstrategy);
    underlying->relation_vacuum(rel, params, bstrategy);
}

static bool onesparse_scan_analyze_next_block(TableScanDesc scan, ReadStream *stream)
{
    TAMLOG("scan=%p, stream=%p", (void *) scan, (void *) stream);
    return underlying->scan_analyze_next_block(scan, stream);
}

static bool onesparse_scan_analyze_next_tuple(TableScanDesc scan,
                                              TransactionId OldestXmin,
                                              double *liverows,
                                              double *deadrows,
                                              TupleTableSlot *slot)
{
    TAMLOG("scan=%p, OldestXmin=%u, slot=%p", (void *) scan, OldestXmin, (void *) slot);
    return underlying->scan_analyze_next_tuple(scan, OldestXmin, liverows, deadrows, slot);
}

static double onesparse_index_build_range_scan(Relation table_rel, Relation index_rel,
                                               struct IndexInfo *index_info,
                                               bool allow_sync, bool anyvisible,
                                               bool progress,
                                               BlockNumber start_blockno,
                                               BlockNumber numblocks,
                                               IndexBuildCallback callback,
                                               void *callback_state,
                                               TableScanDesc scan)
{
    TAMLOG("table_rel=%s, index_rel=%s, allow_sync=%d, anyvisible=%d, progress=%d",
         RelationGetRelationName(table_rel), RelationGetRelationName(index_rel),
         (int) allow_sync, (int) anyvisible, (int) progress);
    return underlying->index_build_range_scan(table_rel, index_rel, index_info,
                                              allow_sync, anyvisible, progress,
                                              start_blockno, numblocks,
                                              callback, callback_state, scan);
}

static void onesparse_index_validate_scan(Relation table_rel, Relation index_rel,
                                          struct IndexInfo *index_info,
                                          Snapshot snapshot,
                                          struct ValidateIndexState *state)
{
    TAMLOG("table_rel=%s, index_rel=%s, snapshot=%p", RelationGetRelationName(table_rel),
         RelationGetRelationName(index_rel), (void *) snapshot);
    underlying->index_validate_scan(table_rel, index_rel, index_info, snapshot, state);
}

static uint64 onesparse_relation_size(Relation rel, ForkNumber forkNumber)
{
    TAMLOG("rel=%s, forkNumber=%d", RelationGetRelationName(rel), forkNumber);
    return underlying->relation_size(rel, forkNumber);
}

static bool onesparse_relation_needs_toast_table(Relation rel)
{
    TAMLOG("rel=%s", RelationGetRelationName(rel));
    return underlying->relation_needs_toast_table(rel);
}

static Oid onesparse_relation_toast_am(Relation rel)
{
    TAMLOG("rel=%s", RelationGetRelationName(rel));
    return underlying->relation_toast_am(rel);
}

static void onesparse_relation_fetch_toast_slice(Relation toastrel, Oid valueid,
                                                 int32 attrsize, int32 sliceoffset,
                                                 int32 slicelength, struct varlena *result)
{
    TAMLOG("toastrel=%s, valueid=%u, attrsize=%d, sliceoffset=%d, slicelength=%d",
         RelationGetRelationName(toastrel), valueid, attrsize, sliceoffset, slicelength);
    underlying->relation_fetch_toast_slice(toastrel, valueid, attrsize, sliceoffset, slicelength, result);
}

static TableAmRoutine *
get_onesparse_routine(void)
{
    MemoryContext old;
    if (onesparse_routine)
        return onesparse_routine;

    underlying = GetHeapamTableAmRoutine();
    old = MemoryContextSwitchTo(TopMemoryContext);
    onesparse_routine = (TableAmRoutine *) palloc0(sizeof(TableAmRoutine));
    MemoryContextSwitchTo(old);

    memcpy(onesparse_routine, underlying, sizeof(TableAmRoutine));

    onesparse_routine->slot_callbacks = onesparse_slot_callbacks;
    onesparse_routine->scan_begin = onesparse_scan_begin;
    onesparse_routine->scan_end = onesparse_scan_end;
    onesparse_routine->scan_rescan = onesparse_scan_rescan;
    onesparse_routine->scan_getnextslot = onesparse_scan_getnextslot;
    onesparse_routine->scan_set_tidrange = onesparse_scan_set_tidrange;
    onesparse_routine->scan_getnextslot_tidrange = onesparse_scan_getnextslot_tidrange;
    onesparse_routine->parallelscan_estimate = onesparse_parallelscan_estimate;
    onesparse_routine->parallelscan_initialize = onesparse_parallelscan_initialize;
    onesparse_routine->parallelscan_reinitialize = onesparse_parallelscan_reinitialize;
    onesparse_routine->index_fetch_begin = onesparse_index_fetch_begin;
    onesparse_routine->index_fetch_reset = onesparse_index_fetch_reset;
    onesparse_routine->index_fetch_end = onesparse_index_fetch_end;
    onesparse_routine->index_fetch_tuple = onesparse_index_fetch_tuple;
    onesparse_routine->tuple_fetch_row_version = onesparse_tuple_fetch_row_version;
    onesparse_routine->tuple_tid_valid = onesparse_tuple_tid_valid;
    onesparse_routine->tuple_get_latest_tid = onesparse_tuple_get_latest_tid;
    onesparse_routine->tuple_satisfies_snapshot = onesparse_tuple_satisfies_snapshot;
    onesparse_routine->index_delete_tuples = onesparse_index_delete_tuples;
    onesparse_routine->tuple_insert = onesparse_tuple_insert;
    onesparse_routine->tuple_insert_speculative = onesparse_tuple_insert_speculative;
    onesparse_routine->tuple_complete_speculative = onesparse_tuple_complete_speculative;
    onesparse_routine->multi_insert = onesparse_multi_insert;
    onesparse_routine->tuple_delete = onesparse_tuple_delete;
    onesparse_routine->tuple_update = onesparse_tuple_update;
    onesparse_routine->tuple_lock = onesparse_tuple_lock;
    onesparse_routine->finish_bulk_insert = onesparse_finish_bulk_insert;
    onesparse_routine->relation_set_new_filelocator = onesparse_relation_set_new_filelocator;
    onesparse_routine->relation_nontransactional_truncate = onesparse_relation_nontransactional_truncate;
    onesparse_routine->relation_copy_data = onesparse_relation_copy_data;
    onesparse_routine->relation_copy_for_cluster = onesparse_relation_copy_for_cluster;
    onesparse_routine->relation_vacuum = onesparse_relation_vacuum;
    onesparse_routine->scan_analyze_next_block = onesparse_scan_analyze_next_block;
    onesparse_routine->scan_analyze_next_tuple = onesparse_scan_analyze_next_tuple;
    onesparse_routine->index_build_range_scan = onesparse_index_build_range_scan;
    onesparse_routine->index_validate_scan = onesparse_index_validate_scan;
    onesparse_routine->relation_size = onesparse_relation_size;
    onesparse_routine->relation_needs_toast_table = onesparse_relation_needs_toast_table;
    onesparse_routine->relation_toast_am = onesparse_relation_toast_am;
    onesparse_routine->relation_fetch_toast_slice = onesparse_relation_fetch_toast_slice;

    return onesparse_routine;
}

PG_FUNCTION_INFO_V1(onesparse_tam_handler);
Datum
onesparse_tam_handler(PG_FUNCTION_ARGS)
{
    TableAmRoutine *r = get_onesparse_routine();
    PG_RETURN_POINTER(r);
}
