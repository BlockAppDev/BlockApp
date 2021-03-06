package com.focuskeeper.test;

import static org.hamcrest.CoreMatchers.*;
import static org.hamcrest.MatcherAssert.assertThat;
import java.util.LinkedHashMap;
import java.util.Map;
import org.junit.Test;
import com.focuskeeper.DatabaseController;

public class TestLoop1 {
	static final String URL = "www.facebook.com";
	static final String URL_2 = "www.instagram.com";
	
	@Test
	public void testRecentlyUsedLoopTwice() {
		DatabaseController.restartDB();
		DatabaseController.connect();
		DatabaseController.createTable();
		DatabaseController.addURLUsage(33, URL);
		DatabaseController.addURLUsage(4, URL_2);
		Map<String, Integer> output = DatabaseController.getRecentlyUsed();
	    Map<String, Integer> expected = new LinkedHashMap<>();
	    expected.put(URL, 33);
	    expected.put(URL_2, 4);
		//Test equal, ignore order
	    assertThat(output, is(expected));
	}
	
	@Test
	public void testRecentlyUsedLoopZero() {
		DatabaseController.restartDB();
		DatabaseController.connect();
		DatabaseController.createTable();
		Map<String, Integer> output = DatabaseController.getRecentlyUsed();
	    Map<String, Integer> expected = new LinkedHashMap<>();
		//Test equal, ignore order
	    assertThat(output, is(expected));
	}
}
