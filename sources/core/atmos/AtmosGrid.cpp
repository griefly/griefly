#include "AtmosGrid.h"

#include "emmintrin.h"

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#endif // __GNUC

#define LIBDIVIDE_USE_SSE2 1
#include "thirdparty/libdivide/libdivide.h"

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif // __GNUC

void AtmosGrid::Process()
{
    ProcessGroups();
    ProcessGroupsBorders();
    Finalize();
}

namespace
{
    libdivide::divider<int> divider_1(1);
    libdivide::divider<int> divider_2(2);
    libdivide::divider<int> divider_3(3);
    libdivide::divider<int> divider_4(4);
    libdivide::divider<int> divider_5(5);
}

inline int hsum_epi32_sse2(__m128i x) {
#ifdef __AVX__
    __m128i hi64  = _mm_unpackhi_epi64(x, x);           // 3-operand non-destructive AVX lets us save a byte without needing a mov
#else
    __m128i hi64  = _mm_shuffle_epi32(x, _MM_SHUFFLE(1, 0, 3, 2));
#endif
    __m128i sum64 = _mm_add_epi32(hi64, x);
    __m128i hi32  = _mm_shufflelo_epi16(sum64, _MM_SHUFFLE(1, 0, 3, 2));    // Swap the low two elements
    __m128i sum32 = _mm_add_epi32(sum64, hi32);
    return _mm_cvtsi128_si32(sum32);       // SSE2 movd
    //return _mm_extract_epi32(hl, 0);     // SSE4, even though it compiles to movd instead of a literal pextrd r32,xmm,0
}

//#define NOTDEF

#ifdef NOTDEF
inline void ProcessFiveCells(AtmosGrid::Cell* __restrict near_cells[])
{
    int near_size = 0;
    __m128i gases_sums_sse = _mm_setzero_si128();
    int* gases_sums = reinterpret_cast<int*>(&gases_sums_sse);
    int energy_sum = 0;

    for (int dir = 0; dir < atmos::DIRS_SIZE + 1; ++dir)
    {
        if (AtmosGrid::Cell* nearby = near_cells[dir])
        {
            __m128i nearby_gases_sse = _mm_load_si128( reinterpret_cast<__m128i*>(nearby->data.gases));
            gases_sums_sse = _mm_add_epi32(nearby_gases_sse, gases_sums_sse);
            energy_sum += nearby->data.energy;
            ++near_size;
        }
    }

    int gases_average[GASES_NUM] alignas(16);
    __m128i* gases_average_sse = reinterpret_cast<__m128i*>(gases_average);
    int gases_remains[GASES_NUM] alignas(16);
    __m128i* gases_remains_sse = reinterpret_cast<__m128i*>(gases_remains);

    int energy_average;
    int energy_remains;
    switch (near_size)
    {
    case 1:
        *gases_average_sse = gases_sums_sse;
        *gases_remains_sse = _mm_setzero_si128();
        energy_average = energy_sum;
        energy_remains = 0;
        break;
    case 2:
        *gases_average_sse = divider_2.perform_divide_vector(gases_sums_sse);
        *gases_remains_sse = _mm_setzero_si128();
        energy_average = divider_2.perform_divide(energy_sum);
        energy_remains = energy_sum - energy_average * 2;
        break;
    case 3:
        *gases_average_sse = divider_3.perform_divide_vector(gases_sums_sse);
        *gases_remains_sse = _mm_setzero_si128();
        energy_average = divider_3.perform_divide(energy_sum);
        energy_remains = energy_sum - energy_average * 3;
        break;
    case 4:
        *gases_average_sse = divider_4.perform_divide_vector(gases_sums_sse);
        *gases_remains_sse = _mm_setzero_si128();
        energy_average = divider_4.perform_divide(energy_sum);
        energy_remains = energy_sum - energy_average * 4;
        break;
    case 5:
        *gases_average_sse = divider_5.perform_divide_vector(gases_sums_sse);
        *gases_remains_sse = _mm_setzero_si128();
        energy_average = divider_5.perform_divide(energy_sum);
        energy_remains = energy_sum - energy_average * 5;
        break;
    }


    /*for (int i = 0; i < GASES_NUM; ++i)
    {
        if (gases_sums[i] == 0)
        {
            continue;
        }
        qDebug() << "Near size:" << near_size;
        qDebug() << "Sums:" << gases_sums[i];
        qDebug() << "Average:" << gases_average[i];
        qDebug() << "Remains:" << gases_remains[i];
    }*/

    /*int gases_average[GASES_NUM];
    int gases_remains[GASES_NUM];
    for (int i = 0; i < GASES_NUM; ++i)
    {
        gases_average[i] = gases_sums[i] / near_size;
        gases_remains[i] = gases_sums[i] % near_size;
    }
    int energy_average = energy_sum / near_size;
    int energy_remains = energy_sum % near_size;*/

    AtmosGrid::Cell& center = *near_cells[atmos::DIRS_SIZE];
    __m128i* center_gases_sse = reinterpret_cast<__m128i*>(center.data.gases);
    for (int dir = 0; dir < atmos::DIRS_SIZE; ++dir)
    {
        if (near_cells[dir])
        {
            AtmosGrid::Cell& nearby = *near_cells[dir];
            __m128i* nearby_gases_sse = reinterpret_cast<__m128i*>(nearby.data.gases);
            __m128i diff = _mm_sub_epi32(*gases_average_sse, *nearby_gases_sse);
            int diff_sum = hsum_epi32_sse2(diff);
            *nearby_gases_sse = *gases_average_sse;
            nearby.flows[atmos::REVERT_DIRS_INDEXES[dir]] += diff_sum;
            center.flows[dir] -= diff_sum;
            nearby.data.energy = energy_average;
        }
    }

    *center_gases_sse = *gases_average_sse;
    center.data.energy = energy_average + energy_remains;
}
#else
inline void ProcessFiveCells(AtmosGrid::Cell* near_cells[])
{
    int near_size = 0;
    int gases_sums[GASES_NUM];
    for (int i = 0; i < GASES_NUM; ++i)
    {
        gases_sums[i] = 0;
    }
    int energy_sum = 0;

    for (int dir = 0; dir < atmos::DIRS_SIZE + 1; ++dir)
    {
        if (AtmosGrid::Cell* nearby = near_cells[dir])
        {
            for (int i = 0; i < GASES_NUM; ++i)
            {
                gases_sums[i] += nearby->data.gases[i];
            }
            energy_sum += nearby->data.energy;
            ++near_size;
        }
    }

    int gases_average[GASES_NUM];
    int gases_remains[GASES_NUM];
    for (int i = 0; i < GASES_NUM; ++i)
    {
        gases_average[i] = gases_sums[i] / near_size;
        gases_remains[i] = gases_sums[i] % near_size;
    }
    int energy_average = energy_sum / near_size;
    int energy_remains = energy_sum % near_size;

    AtmosGrid::Cell& center = *near_cells[atmos::DIRS_SIZE];

    for (int dir = 0; dir < atmos::DIRS_SIZE; ++dir)
    {
        if (near_cells[dir])
        {
            AtmosGrid::Cell& nearby = *near_cells[dir];
            for (int i = 0; i < GASES_NUM; ++i)
            {
                int diff = gases_average[i] - nearby.data.gases[i];
                nearby.flows[atmos::REVERT_DIRS_INDEXES[dir]] += diff;
                center.flows[dir] -= diff;
                nearby.data.gases[i] = gases_average[i];
            }
            nearby.data.energy = energy_average;
        }
    }

    for (int i = 0; i < GASES_NUM; ++i)
    {
        center.data.gases[i] = gases_average[i] + gases_remains[i];
    }
    center.data.energy = energy_average + energy_remains;
}
#endif // NOTDEF

inline AtmosGrid::Cell& GetNearInGroup(AtmosGrid::Cell* current, Dir dir)
{
    switch (dir)
    {
    case D_DOWN:
        return *(current + 1);
    case D_UP:
        return *(current - 1);
    case D_RIGHT:
        return *(current + atmos::CELL_GROUP_SIZE);
    case D_LEFT:
        return *(current - atmos::CELL_GROUP_SIZE);
    }
}

namespace
{
    const int AMOUNT_CELLS_IN_GROUP = atmos::CELL_GROUP_SIZE * atmos::CELL_GROUP_SIZE;
    const int STAGES_AMOUNT = 5;
    inline bool BelongsToStage(int x, int y, int stage)
    {
        return (((x + (y * 2)) % STAGES_AMOUNT) == ((MAIN_TICK + stage) % STAGES_AMOUNT));
    }

    inline void ProcessInnerGroupCell(AtmosGrid::Cell* current)
    {
        if (!current->IsPassable(atmos::CENTER))
        {
            return;
        }

        AtmosGrid::Cell* near_cells[atmos::DIRS_SIZE + 1];

        for (int dir = 0; dir < atmos::DIRS_SIZE; ++dir)
        {
            if (current->IsPassable(atmos::DIRS[dir]))
            {
                AtmosGrid::Cell& nearby = GetNearInGroup(current, atmos::INDEXES_TO_DIRS[dir]);
                if (   nearby.IsPassable(atmos::REVERT_DIRS[dir])
                    && nearby.IsPassable(atmos::CENTER))
                {
                    near_cells[dir] = &nearby;
                    continue;
                }
            }
            near_cells[dir] = nullptr;
        }
        near_cells[atmos::DIRS_SIZE] = current;

        ProcessFiveCells(near_cells);
    }
}
inline void AtmosGrid::ProcessBorderGroupCell(Cell* current, int x, int y)
{
    if (!current->IsPassable(atmos::CENTER))
    {
        return;
    }

    Cell* near_cells[atmos::DIRS_SIZE + 1];

    for (int dir = 0; dir < atmos::DIRS_SIZE; ++dir)
    {
        if (current->IsPassable(atmos::DIRS[dir]))
        {
            Cell& nearby = Get(x, y, atmos::INDEXES_TO_DIRS[dir]);
            if (   nearby.IsPassable(atmos::REVERT_DIRS[dir])
                && nearby.IsPassable(atmos::CENTER))
            {
                near_cells[dir] = &nearby;
                continue;
            }
        }
        near_cells[dir] = nullptr;
    }
    near_cells[atmos::DIRS_SIZE] = current;

    ProcessFiveCells(near_cells);
}

void AtmosGrid::ProcessGroups()
{
    Cell* current_group = &cells_[0];
    const int GROUPS_AMOUNT = group_height_ * group_width_;
    for (int index = 0; index < GROUPS_AMOUNT; ++index, current_group += AMOUNT_CELLS_IN_GROUP)
    {
        for (int stage = 0; stage < STAGES_AMOUNT; ++stage)
        {
            for (int x = 1; x < atmos::CELL_GROUP_SIZE - 1; ++x)
            {
                for (int y = 1; y < atmos::CELL_GROUP_SIZE - 1; ++y)
                {
                    if (!BelongsToStage(x, y, stage))
                    {
                        continue;
                    }
                    Cell& current = current_group[y + x * atmos::CELL_GROUP_SIZE];
                    ProcessInnerGroupCell(&current);
                }
            }
        }
    }
}

void AtmosGrid::ProcessGroupsBorders()
{
    for (int group_x = 1; group_x < group_width_ - 1; ++group_x)
    {
        int end_x = group_x * atmos::CELL_GROUP_SIZE;
        int start_x = end_x - 1;
        for (int stage = 0; stage < STAGES_AMOUNT; ++stage)
        {
            for (int x = start_x; x <= end_x; ++x)
            {
                for (int y = 1; y < height_ - 1; ++y)
                {
                    if (!BelongsToStage(x, y, stage))
                    {
                        continue;
                    }
                    Cell& current = At(x, y);
                    ProcessBorderGroupCell(&current, x, y);
                }
            }
        }
    }
    for (int group_y = 1; group_y < group_height_ - 1; ++group_y)
    {
        int end_y = group_y * atmos::CELL_GROUP_SIZE;
        int start_y = end_y - 1;
        for (int stage = 0; stage < STAGES_AMOUNT; ++stage)
        {
            for (int y = start_y; y <= end_y; ++y)
            {
                for (int x = 1; x < width_ - 1; ++x)
                {
                    if (!BelongsToStage(x, y, stage))
                    {
                        continue;
                    }
                    Cell& current = At(x, y);
                    ProcessBorderGroupCell(&current, x, y);
                }
            }
        }
    }
}

void AtmosGrid::Finalize()
{
    for (int pos = 0; pos < length_; ++pos)
    {
        Cell& cell = cells_[pos];

        if (cell.flags & atmos::SPACE)
        {
            for (int i = 0; i < GASES_NUM; ++i)
            {
                cell.data.gases[i] *= 4;
                cell.data.gases[i] /= 5;
            }
            cell.data.energy *= 4;
            cell.data.energy /= 5;
        }

        UpdateMacroParams(&cell.data);
    }
}
