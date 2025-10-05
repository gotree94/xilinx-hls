`timescale 1ns/1ps

module aes128_tb;

    // Clock and reset
    reg clk;
    reg rst;
    reg start;
    
    // Test vectors
    reg [127:0] plaintext;
    reg [127:0] key;
    wire [127:0] ciphertext;
    wire done;
    
    // Test counters
    integer test_num;
    integer pass_count;
    integer fail_count;
    
    // DUT instantiation
    aes128_encrypt dut (
        .clk(clk),
        .rst(rst),
        .start(start),
        .plaintext(plaintext),
        .key(key),
        .ciphertext(ciphertext),
        .done(done)
    );
    
    // Clock generation: 10ns period (100MHz)
    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end
    
    // Test procedure
    initial begin
        $display("");
        $display("================================================================================");
        $display("                    AES-128 Verilog Implementation Test");
        $display("================================================================================");
        $display("");
        
        // Initialize counters
        test_num = 0;
        pass_count = 0;
        fail_count = 0;
        
        // Reset
        rst = 1;
        start = 0;
        plaintext = 128'h0;
        key = 128'h0;
        
        repeat(3) @(posedge clk);
        rst = 0;
        repeat(2) @(posedge clk);
        
        //=================================================================
        // Test 1: NIST FIPS-197 Standard Test Vector
        //=================================================================
        test_num = test_num + 1;
        $display("Test %0d: NIST FIPS-197 Standard Test Vector", test_num);
        $display("--------------------------------------------------------------------------------");
        
        plaintext = 128'h3243f6a8885a308d313198a2e0370734;
        key = 128'h2b7e151628aed2a6abf7158809cf4f3c;
        
        $display("Input:");
        $display("  Plaintext = %032x", plaintext);
        $display("  Key       = %032x", key);
        
        // Start encryption
        @(posedge clk);
        start = 1;
        @(posedge clk);
        start = 0;
        
        // Wait for done
        wait(done == 1);
        @(posedge clk);
        
        $display("Output:");
        $display("  Ciphertext = %032x", ciphertext);
        $display("  Expected   = 3925841d02dc09fbdc118597196a0b32");
        
        if (ciphertext == 128'h3925841d02dc09fbdc118597196a0b32) begin
            $display("  Result: PASS ✓");
            pass_count = pass_count + 1;
        end else begin
            $display("  Result: FAIL ✗");
            fail_count = fail_count + 1;
        end
        $display("");
        
        repeat(10) @(posedge clk);
        
        //=================================================================
        // Test 2: All Zeros
        //=================================================================
        test_num = test_num + 1;
        $display("Test %0d: All Zeros Plaintext and Key", test_num);
        $display("--------------------------------------------------------------------------------");
        
        plaintext = 128'h00000000000000000000000000000000;
        key = 128'h00000000000000000000000000000000;
        
        $display("Input:");
        $display("  Plaintext = %032x", plaintext);
        $display("  Key       = %032x", key);
        
        @(posedge clk);
        start = 1;
        @(posedge clk);
        start = 0;
        
        wait(done == 1);
        @(posedge clk);
        
        $display("Output:");
        $display("  Ciphertext = %032x", ciphertext);
        $display("  Expected   = 66e94bd4ef8a2c3b884cfa59ca342b2e");
        
        if (ciphertext == 128'h66e94bd4ef8a2c3b884cfa59ca342b2e) begin
            $display("  Result: PASS ✓");
            pass_count = pass_count + 1;
        end else begin
            $display("  Result: FAIL ✗");
            fail_count = fail_count + 1;
        end
        $display("");
        
        repeat(10) @(posedge clk);
        
        //=================================================================
        // Test 3: All Ones
        //=================================================================
        test_num = test_num + 1;
        $display("Test %0d: All Ones Plaintext and Key", test_num);
        $display("--------------------------------------------------------------------------------");
        
        plaintext = 128'hffffffffffffffffffffffffffffffff;
        key = 128'hffffffffffffffffffffffffffffffff;
        
        $display("Input:");
        $display("  Plaintext = %032x", plaintext);
        $display("  Key       = %032x", key);
        
        @(posedge clk);
        start = 1;
        @(posedge clk);
        start = 0;
        
        wait(done == 1);
        @(posedge clk);
        
        $display("Output:");
        $display("  Ciphertext = %032x", ciphertext);
        $display("  Expected   = a1f6258c877d5fcd8964484538bfc92c");
        
        if (ciphertext == 128'ha1f6258c877d5fcd8964484538bfc92c) begin
            $display("  Result: PASS ✓");
            pass_count = pass_count + 1;
        end else begin
            $display("  Result: FAIL ✗");
            fail_count = fail_count + 1;
        end
        $display("");
        
        repeat(10) @(posedge clk);
        
        //=================================================================
        // Test 4: Pattern Test
        //=================================================================
        test_num = test_num + 1;
        $display("Test %0d: Pattern Plaintext", test_num);
        $display("--------------------------------------------------------------------------------");
        
        plaintext = 128'h00112233445566778899aabbccddeeff;
        key = 128'h000102030405060708090a0b0c0d0e0f;
        
        $display("Input:");
        $display("  Plaintext = %032x", plaintext);
        $display("  Key       = %032x", key);
        
        @(posedge clk);
        start = 1;
        @(posedge clk);
        start = 0;
        
        wait(done == 1);
        @(posedge clk);
        
        $display("Output:");
        $display("  Ciphertext = %032x", ciphertext);
        $display("  Expected   = 69c4e0d86a7b0430d8cdb78070b4c55a");
        
        if (ciphertext == 128'h69c4e0d86a7b0430d8cdb78070b4c55a) begin
            $display("  Result: PASS ✓");
            pass_count = pass_count + 1;
        end else begin
            $display("  Result: FAIL ✗");
            fail_count = fail_count + 1;
        end
        $display("");
        
        repeat(10) @(posedge clk);
        
        //=================================================================
        // Test Summary
        //=================================================================
        $display("================================================================================");
        $display("                              Test Summary");
        $display("================================================================================");
        $display("  Total Tests:  %0d", test_num);
        $display("  Passed:       %0d", pass_count);
        $display("  Failed:       %0d", fail_count);
        $display("================================================================================");
        $display("");
        
        if (fail_count == 0) begin
            $display("  ███████████████████████████████████████");
            $display("  █                                     █");
            $display("  █     ALL TESTS PASSED! ✓ ✓ ✓        █");
            $display("  █                                     █");
            $display("  ███████████████████████████████████████");
        end else begin
            $display("  ███████████████████████████████████████");
            $display("  █                                     █");
            $display("  █     SOME TESTS FAILED! ✗            █");
            $display("  █                                     █");
            $display("  ███████████████████████████████████████");
        end
        $display("");
        
        repeat(10) @(posedge clk);
        $finish;
    end
    
    // Timeout watchdog (prevent infinite simulation)
    initial begin
        #1000000; // 1ms timeout
        $display("");
        $display("ERROR: Simulation timeout!");
        $display("The design may be stuck in a state.");
        $display("");
        $finish;
    end
    
    // Optional: VCD dump for waveform viewing
    initial begin
        $dumpfile("aes128_tb.vcd");
        $dumpvars(0, aes128_tb);
    end
    
    // Optional: Monitor state transitions
    always @(posedge clk) begin
        if (start)
            $display("[%0t] Start signal asserted", $time);
        if (done)
            $display("[%0t] Encryption completed", $time);
    end

endmodule