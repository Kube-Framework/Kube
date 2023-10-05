/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Unit tests of Audio
 */

#include <gtest/gtest.h>

#include <Kube/Audio/Manager.hpp>

using namespace kF;

#include <Kube/Core/Log.hpp>

TEST(Manager, DriverBasics)
{
    Audio::Manager manager;
    const auto drivers = manager.getAvailableDrivers();

    // There must not be zero drivers
    ASSERT_NE(drivers.size(), 0);

    // Try to set drivers
    for (auto i = 0u; i != drivers.size(); ++i) {
        manager.setDriver(drivers[i]);
        ASSERT_EQ(manager.driver(), drivers[i]);
    }

    // Test driver comparison
    for (auto i = 1u; i < drivers.size(); ++i) {
        ASSERT_NE(drivers[i - 1], drivers[i]);
    }
}