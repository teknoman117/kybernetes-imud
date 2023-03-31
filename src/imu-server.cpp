/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "ICM_20948.h"

// "F" macro interferes with asio
#undef F

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/local/stream_protocol.hpp>
#include <asio/signal_set.hpp>
#include <asio/steady_timer.hpp>

#include <array>
#include <chrono>
#include <cstdio>

extern "C" {
    #include <sys/signal.h>
}

struct UnixSocketPathWrangler {
    const char* xdgRuntimeDir;
    std::string path;

    UnixSocketPathWrangler() {
        xdgRuntimeDir = std::getenv("XDG_RUNTIME_DIR");
        path = xdgRuntimeDir != nullptr
            ? std::string(xdgRuntimeDir) + "/imu.sock"
            : "/run/imu.sock";
    }
    ~UnixSocketPathWrangler() {
        std::remove(path.c_str());
    }
} wrangler;

asio::io_context io;
asio::steady_timer imuTimer(io);
asio::signal_set signalReload(io, SIGHUP);
asio::signal_set signalTerminate(io, SIGTERM, SIGINT);

asio::ip::tcp::acceptor acceptorTCPIPv4(io, {asio::ip::tcp::v4(), 4000}, true);
std::vector<asio::ip::tcp::socket> clientsTCPIPv4 = {};

asio::local::stream_protocol::acceptor acceptorUNIX(io, {wrangler.path}, true);
std::vector<asio::local::stream_protocol::socket> clientsUNIX = {};

ICM_20948_I2C imu;

// Process all pending IMU events
void handleIMUTimer(const asio::error_code& ec) {
    icm_20948_DMP_data_t data;

    imu.readDMPdataFromFIFO(&data);
    if ((imu.status == ICM_20948_Stat_Ok)
            || (imu.status == ICM_20948_Stat_FIFOMoreDataAvail)) {
        if (data.header & DMP_header_bitmap_Quat9) {
            // Convert fixed point to floating point (divide by 2^30)
            double q1 = (double) data.Quat9.Data.Q1 / 1073741824.0;
            double q2 = (double) data.Quat9.Data.Q2 / 1073741824.0;
            double q3 = (double) data.Quat9.Data.Q3 / 1073741824.0;
            double q0 = sqrt(1.0 - ((q1 * q1) + (q2 * q2) + (q3 * q3)));

            // Compute magnetic heading
            double siny_cosp = 2.0 * ((q0 * q3) + (q1 * q2));
            double cosy_cosp = 1.0 - 2.0 * ((q2 * q2) + (q3 * q3));
            double heading = -std::atan2(siny_cosp, cosy_cosp) * 180.0 / M_PI;
            if (heading < 0) {
                heading = heading + 360.0;
            }

            std::array<char, 256> message;
            int n = snprintf(message.data(), message.size(),
                "{\"orientation\": {\"w\":%.9lf, \"x\":%.9lf, \"y\":%.9lf, \"z\":%.9lf}, "
                "\"orientation_raw\": {\"x\":%u, \"y\":%u, \"z\":%u}, "
                "\"heading\":%.9lf, \"accuracy\":%u}\n",
                q0, q1, q2, q3,
                data.Quat9.Data.Q1, data.Quat9.Data.Q2, data.Quat9.Data.Q3,
                heading, data.Quat9.Data.Accuracy);

            // Send message to IPv4 clients
            for (auto client = clientsTCPIPv4.begin(); client != clientsTCPIPv4.end(); client++) {
                client->async_write_some(asio::const_buffer(message.data(), n + 1),
                        [client] (const asio::error_code& ec, size_t) {
                    // boot the client upon any error
                    if (ec) {
                        printf("<5> Client disconnected (TCPIPv4)\n");
                        clientsTCPIPv4.erase(client);
                    }
                });
            }

            // Send message to UNIX clients
            for (auto client = clientsUNIX.begin(); client != clientsUNIX.end(); client++) {
                client->async_write_some(asio::const_buffer(message.data(), n + 1),
                        [client] (const asio::error_code& ec, size_t) {
                    // boot the client upon any error
                    if (ec) {
                        printf("<5> Client disconnected (UNIX)\n");
                        clientsUNIX.erase(client);
                    }
                });
            }
        }
    } else {
        // DMP updates at 55 Hz (~18.2 ms), what is a sensible polling rate?
        imuTimer.expires_at(imuTimer.expires_at() + std::chrono::milliseconds(5));
    }

    // Schedule next IMU update
    // TODO: is there a way to avoid having to poll the sensor?
    imuTimer.async_wait(handleIMUTimer);
}

void handleSignalReload(const asio::error_code& ec, int signo) {
    // TODO: implement a way to change which i2c address and/or bus we use
    printf("<5> Reloading Configuration\n");
    signalReload.async_wait(handleSignalReload);
}

void handleSignalTerminate(const asio::error_code& ec, int signo) {
    printf("<5> Shutdown Requested\n");
    io.stop();
}

void handleTCPIPv4Accept(const asio::error_code& ec, asio::ip::tcp::socket socket) {
    if (ec) {
        return;
    }

    printf("<5> Client connected (TCP): %s:%u\n",
            socket.remote_endpoint().address().to_string().data(),
            socket.remote_endpoint().port());

    clientsTCPIPv4.emplace_back(std::move(socket));
    acceptorTCPIPv4.async_accept(handleTCPIPv4Accept);
}

void handleUNIXAccept(const asio::error_code& ec, asio::local::stream_protocol::socket socket) {
    if (ec) {
        return;
    }

    printf("<5> Client connected (UNIX)\n");
    clientsUNIX.emplace_back(std::move(socket));
    acceptorUNIX.async_accept(handleUNIXAccept);
}

void setup() {
    // Initialize the IMU
    Wire.begin();
    imu.begin(Wire, 1);
    if (imu.status != ICM_20948_Stat_Ok) {
        fprintf(stderr, "IMU Initialization Failed\n");
        exit(EXIT_FAILURE);
    }

    // Initialize the DMP
    if (imu.initializeDMP() != ICM_20948_Stat_Ok) {
        fprintf(stderr, "ICM_20948::initializeDMP(...) Failed\n");
        exit(EXIT_FAILURE);
    }

    if (imu.enableDMPSensor(INV_ICM20948_SENSOR_ORIENTATION) != ICM_20948_Stat_Ok) {
        fprintf(stderr, "ICM_20948::enableDMPSensor(...) Failed\n");
        exit(EXIT_FAILURE);
    }

    if (imu.setDMPODRrate(DMP_ODR_Reg_Quat9, 0) != ICM_20948_Stat_Ok) {
        fprintf(stderr, "ICM_20948::setDMPODRrate(...) Failed\n");
        exit(EXIT_FAILURE);
    }

    if (imu.enableFIFO() != ICM_20948_Stat_Ok) {
        fprintf(stderr, "ICM_20948::enableFIFO() Failed\n");
        exit(EXIT_FAILURE);
    }

    if (imu.enableDMP() != ICM_20948_Stat_Ok) {
        fprintf(stderr, "ICM_20948::enableDMP() Failed\n");
        exit(EXIT_FAILURE);
    }

    if (imu.resetDMP() != ICM_20948_Stat_Ok) {
        fprintf(stderr, "ICM_20948::resetDMP(...) Failed\n");
        exit(EXIT_FAILURE);
    }

    if (imu.resetFIFO() != ICM_20948_Stat_Ok) {
        fprintf(stderr, "ICM_20948::resetFIFO(...) Failed\n");
        exit(EXIT_FAILURE);
    }

    // Listen to SIGHUP to reload configuration
    signalReload.async_wait(handleSignalReload);

    // Listen to SIGTERM to terminate the process
    signalTerminate.async_wait(handleSignalTerminate);

    // Listen for network clients
    acceptorTCPIPv4.async_accept(handleTCPIPv4Accept);
    acceptorUNIX.async_accept(handleUNIXAccept);

    // Schedule IMU Update
    imuTimer.expires_at(asio::steady_timer::clock_type::now());
    imuTimer.async_wait(handleIMUTimer);
}

void loop() {
    auto n = io.run_one();
    if (!n) {
        exit(EXIT_SUCCESS);
    }
}
