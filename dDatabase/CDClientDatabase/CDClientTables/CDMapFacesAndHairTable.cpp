#include "CDMapFacesAndHairTable.h"

void CDMapFacesAndHairTable::LoadValuesFromDatabase() {
    unsigned int size = 0;
    auto tableSize = CDClientDatabase::ExecuteQuery("SELECT COUNT(*) FROM mapFacesAndHair");
    while (!tableSize.eof()) {
        size = tableSize.getIntField(0, 0);

        tableSize.nextRow();
    }

    tableSize.finalize();
    auto& entries = GetEntriesMutable();
	entries.reserve(size);

    auto tableData = CDClientDatabase::ExecuteQuery("SELECT * FROM mapFacesAndHair");
    while (!tableData.eof()) {
        CDMapFacesAndHair entry;
        entry.id = tableData.getIntField("id", 0);
        entry.eyes = tableData.getIntField("eyes", 0);
        entry.eyebrows = tableData.getIntField("eyebrows", 0);
        entry.mouth = tableData.getIntField("mouths", 0);
		entry.haircolor = tableData.getIntField("haircolor", 0);
		entry.hairstyle = tableData.getIntField("hairstyle", 0);

        entries.push_back(entry);
        tableData.nextRow();
    }

    tableData.finalize();
}

std::vector<CDMapFacesAndHair> CDMapFacesAndHairTable::Query(std::function<bool(CDMapFacesAndHair)> predicate) {

    std::vector<CDMapFacesAndHair> data = cpplinq::from(GetEntries()) >> cpplinq::where(predicate) >> cpplinq::to_vector();

    return data;
}

CDMapFacesAndHair CDMapFacesAndHairTable::GetByLot(LOT lot) {
    for (const auto& item : GetEntries()) {
        if (item.id == lot) {
            return item;
        }
    }

    return {};
}

CDMapFacesAndHair CDMapFacesAndHairTable::GetByEyes(uint32_t id) {
    for (const auto& item : GetEntries()) {
        if (item.eyes == id) {
            return item;
        }
    }

    return {};
}

CDMapFacesAndHair CDMapFacesAndHairTable::GetByEyebrows(uint32_t id) {
    for (const auto& item : GetEntries()) {
        if (item.eyebrows == id) {
            return item;
        }
    }

    return {};
}

CDMapFacesAndHair CDMapFacesAndHairTable::GetByMouth(uint32_t id) {
    for (const auto& item : GetEntries()) {
        if (item.mouth == id) {
            return item;
        }
    }

    return {};
}
